#include "Translator.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <sstream>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace myvm::translator
{
namespace
{
struct Line { std::size_t number; std::string text; };

std::string lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string trim(std::string value)
{
    const auto first = value.find_first_not_of(" \t\r");
    if (first == std::string::npos) return {};
    const auto last = value.find_last_not_of(" \t\r");
    return value.substr(first, last - first + 1);
}

std::vector<std::string> splitOperands(const std::string& text, std::size_t line)
{
    std::vector<std::string> result;
    std::string current;
    int brackets = 0;
    for (const char c : text)
    {
        if (c == '[') ++brackets;
        if (c == ']') --brackets;
        if (c == ',' && brackets == 0)
        {
            result.push_back(trim(current));
            current.clear();
        }
        else current.push_back(c);
    }
    if (!trim(current).empty()) result.push_back(trim(current));
    if (brackets != 0) throw TranslationError(line, "未闭合的内存操作数");
    return result;
}

bool isRegister(const std::string& operand)
{
    if (operand.size() < 2 || operand[0] != 'r') return false;
    return std::all_of(operand.begin() + 1, operand.end(), [](unsigned char c) { return std::isdigit(c); });
}

std::string registerId(const std::string& operand, std::size_t line)
{
    if (!isRegister(operand)) throw TranslationError(line, "寄存器必须写成 r0、r1 等形式: " + operand);
    return operand.substr(1);
}

bool isImmediate(const std::string& operand)
{
    if (operand.empty()) return false;
    if (operand[0] == '-' || operand[0] == '+') return operand.size() > 1;
    return std::isdigit(static_cast<unsigned char>(operand[0])) ||
           (operand.size() > 2 && operand[0] == '0' && (operand[1] == 'x' || operand[1] == 'X'));
}

std::string immediate(const std::string& operand, std::size_t line)
{
    if (!isImmediate(operand)) throw TranslationError(line, "需要立即数或寄存器: " + operand);
    if (operand[0] == '-') throw TranslationError(line, "当前后端暂不支持负立即数: " + operand);
    if (operand.size() > 2 && operand[0] == '0' && (operand[1] == 'x' || operand[1] == 'X'))
        return operand;
    if (std::all_of(operand.begin(), operand.end(), [](unsigned char c) { return std::isdigit(c); }))
        return "d" + operand;
    return operand;
}

std::string loadImmediate(const std::string& value)
{
    return "    ldl #" + value + "\n    ldh #0\n";
}

std::string loadRegister(const std::string& reg)
{
    return "    leareg " + reg + ", t1\n    ld t1\n";
}

std::string storeRegister(const std::string& reg)
{
    return "    leareg " + reg + ", t0\n    save t0\n";
}

void requireCount(const std::vector<std::string>& operands, std::size_t count,
                  std::size_t line, const std::string& mnemonic)
{
    if (operands.size() != count)
        throw TranslationError(line, mnemonic + " 需要 " + std::to_string(count) + " 个操作数");
}
}

TranslationError::TranslationError(std::size_t line, std::string message)
    : std::runtime_error("第 " + std::to_string(line) + " 行: " + message), line_(line) {}

std::string Translator::translate(const std::string& source, const std::string&)
{
    std::vector<Line> lines;
    std::istringstream input(source);
    std::string raw;
    std::size_t lineNumber = 0;
    while (std::getline(input, raw))
    {
        ++lineNumber;
        if (const auto comment = raw.find_first_of(";#"); comment != std::string::npos)
            raw.resize(comment);
        lines.push_back({lineNumber, trim(raw)});
    }

    std::ostringstream output;
    output << ".code\n";
    output << ".const __true = 1\n";
    for (const auto& line : lines)
    {
        if (line.text.empty()) continue;
        auto text = line.text;
        if (text.back() == ':')
        {
            output << text << '\n';
            continue;
        }
        std::istringstream statement(text);
        std::string mnemonic;
        statement >> mnemonic;
        mnemonic = lower(mnemonic);
        std::string operandsText;
        std::getline(statement, operandsText);
        const auto operands = splitOperands(trim(operandsText), line.number);

        if (mnemonic == "mov")
        {
            requireCount(operands, 2, line.number, mnemonic);
            const auto dst = registerId(lower(operands[0]), line.number);
            if (isRegister(lower(operands[1]))) output << loadRegister(registerId(lower(operands[1]), line.number));
            else output << loadImmediate(immediate(operands[1], line.number));
            output << storeRegister(dst);
        }
        else if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "mul" || mnemonic == "div")
        {
            requireCount(operands, 2, line.number, mnemonic);
            const auto dst = registerId(lower(operands[0]), line.number);
            if (isRegister(lower(operands[1]))) output << loadRegister(registerId(lower(operands[1]), line.number));
            else output << loadImmediate(immediate(operands[1], line.number));
            output << "    leareg " << dst << ", t0\n    " << (mnemonic == "sub" ? "dec" : mnemonic)
                   << " t0\n";
        }
        else if (mnemonic == "cmp")
        {
            requireCount(operands, 2, line.number, mnemonic);
            const auto lhs = registerId(lower(operands[0]), line.number);
            if (isRegister(lower(operands[1]))) output << loadRegister(registerId(lower(operands[1]), line.number));
            else output << loadImmediate(immediate(operands[1], line.number));
            output << "    leareg " << lhs << ", t0\n    cmp t0\n";
        }
        else if (mnemonic == "jmp" || mnemonic == "je" || mnemonic == "jg" || mnemonic == "jl")
        {
            requireCount(operands, 1, line.number, mnemonic);
            const auto target = operands[0];
            if (mnemonic == "jmp") output << loadImmediate(target) << "    jmp\n";
            else
            {
                const std::string condition = mnemonic == "je" ? "1" : mnemonic == "jg" ? "2" : "0";
                // cmp leaves the flags intact while loading the branch target into currdata.
                output << loadImmediate(target) << "    asjmp #" << condition << "\n";
            }
        }
        else if (mnemonic == "push")
        {
            requireCount(operands, 1, line.number, mnemonic);
            if (isRegister(lower(operands[0]))) output << loadRegister(registerId(lower(operands[0]), line.number));
            else output << loadImmediate(immediate(operands[0], line.number));
            output << "    push\n";
        }
        else if (mnemonic == "pop")
        {
            requireCount(operands, 1, line.number, mnemonic);
            output << "    pop\n" << storeRegister(registerId(lower(operands[0]), line.number));
        }
        else if (mnemonic == "call")
        {
            requireCount(operands, 1, line.number, mnemonic);
            output << loadImmediate(operands[0]) << "    call\n";
        }
        else if (mnemonic == "ret")
        {
            requireCount(operands, 0, line.number, mnemonic);
            output << "    ret\n";
        }
        else if (mnemonic == "halt" || mnemonic == "pause")
        {
            requireCount(operands, 0, line.number, mnemonic);
            output << "    pause\n";
        }
        else
            throw TranslationError(line.number, "未知源指令: " + mnemonic);
    }
    return output.str();
}
}
