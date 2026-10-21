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
        if (!mnemonic.empty() && mnemonic.front() == '.') continue;
        std::string operandsText;
        std::getline(statement, operandsText);
        const auto operands = splitOperands(trim(operandsText), line.number);
        output << "; @macro line=" << line.number << " op=" << mnemonic << "\n";

        if (mnemonic == "mov")
        {
            requireCount(operands, 2, line.number, mnemonic);
            const auto dst = registerId(lower(operands[0]), line.number);
            if (isRegister(lower(operands[1]))) output << loadRegister(registerId(lower(operands[1]), line.number));
            else output << loadImmediate(immediate(operands[1], line.number));
            output << storeRegister(dst);
        }
        else if (mnemonic == "add" || mnemonic == "sub" || mnemonic == "mul" || mnemonic == "div" ||
                 mnemonic == "and" || mnemonic == "or" || mnemonic == "xor" ||
                 mnemonic == "shl" || mnemonic == "shr")
        {
            requireCount(operands, 2, line.number, mnemonic);
            const auto dst = registerId(lower(operands[0]), line.number);
            if (isRegister(lower(operands[1]))) output << loadRegister(registerId(lower(operands[1]), line.number));
            else output << loadImmediate(immediate(operands[1], line.number));
            output << "    leareg " << dst << ", t0\n    " << (mnemonic == "sub" ? "dec" : mnemonic)
                   << " t0\n";
        }
        else if (mnemonic == "inc" || mnemonic == "not")
        {
            requireCount(operands, 1, line.number, mnemonic);
            const auto dst = registerId(lower(operands[0]), line.number);
            if (mnemonic == "inc") output << loadImmediate("d1");
            else output << loadImmediate("d0");
            output << "    leareg " << dst << ", t0\n    " << (mnemonic == "inc" ? "add" : "not") << " t0\n";
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

std::string Translator::translateZigIntel(const std::string& source, const std::string& sourceName)
{
    std::ostringstream lowered;
    lowered << ".code\n";
    std::istringstream input(source);
    std::vector<std::string> sourceLines;
    std::string raw;
    while (std::getline(input, raw)) sourceLines.push_back(raw);
    std::unordered_map<std::string, std::string> publicAliases;
    for (const auto& sourceLine : sourceLines)
    {
        const auto text = trim(sourceLine);
        const auto equal = text.find(" = .L");
        if (equal != std::string::npos)
            publicAliases[trim(text.substr(equal + 3))] = trim(text.substr(0, equal));
    }
    std::size_t line = 0;
    std::string pendingPublic;
    const std::unordered_map<std::string, std::string> registers = {
        {"rax", "r5"}, {"rbx", "r6"}, {"rcx", "r7"}, {"rdx", "r8"},
        {"rdi", "r9"}, {"rsi", "r10"}, {"rdp", "r11"}, {"rsp", "r12"}
    };
    auto mapRegister = [&](std::string value) {
        value = lower(trim(value));
        const auto found = registers.find(value);
        return found == registers.end() ? std::string{} : found->second;
    };
    for (const auto& sourceLine : sourceLines)
    {
        ++line;
        raw = sourceLine;
        auto text = trim(raw);
        if (text.empty() || text[0] == '#' || text == ".text") continue;
        if (text.rfind(".globl ", 0) == 0)
        {
            pendingPublic = trim(text.substr(7));
            continue;
        }
        if (text.back() == ':')
        {
            if (text.rfind(".L", 0) == 0)
            {
                const auto publicAlias = publicAliases.find(text.substr(0, text.size() - 1));
                if (publicAlias != publicAliases.end())
                    lowered << publicAlias->second << ":\n";
                else if (!pendingPublic.empty())
                {
                    lowered << pendingPublic << ":\n";
                    pendingPublic.clear();
                }
                continue;
            }
            lowered << text << '\n';
            continue;
        }
        if (text[0] == '.') continue;
        if (text.find(" = .L") != std::string::npos)
            continue;
        std::istringstream statement(text);
        std::string mnemonic;
        statement >> mnemonic;
        mnemonic = lower(mnemonic);
        std::string operands;
        std::getline(statement, operands);
        operands = trim(operands);
        if (mnemonic == "push" || mnemonic == "pop") continue; // Zig prologue/epilogue.
        if (mnemonic == "ret") { lowered << "    ret\n"; continue; }
        if (mnemonic == "mov")
        {
            const auto comma = operands.find(',');
            if (comma == std::string::npos) throw TranslationError(line, "Zig Intel mov 缺少逗号");
            const auto dst = mapRegister(operands.substr(0, comma));
            const auto src = mapRegister(operands.substr(comma + 1));
            if (dst.empty() || src.empty()) continue; // 栈帧建立等宿主细节。
            lowered << "    mov " << dst << ", " << src << '\n';
            continue;
        }
        if (mnemonic == "lea")
        {
            const auto comma = operands.find(',');
            if (comma == std::string::npos) throw TranslationError(line, "Zig Intel lea 缺少逗号");
            const auto dst = mapRegister(operands.substr(0, comma));
            auto address = trim(operands.substr(comma + 1));
            if (address.size() < 2 || address.front() != '[' || address.back() != ']')
                throw TranslationError(line, "只支持简单的 Zig Intel [reg + reg] lea");
            address = address.substr(1, address.size() - 2);
            const auto plus = address.find('+');
            if (plus == std::string::npos) throw TranslationError(line, "lea 必须包含两个寄存器");
            const auto lhs = mapRegister(address.substr(0, plus));
            const auto rhs = mapRegister(address.substr(plus + 1));
            if (dst.empty() || lhs.empty() || rhs.empty()) throw TranslationError(line, "不支持的 Zig ABI 寄存器");
            lowered << "    mov " << dst << ", " << lhs << "\n"
                    << "    add " << dst << ", " << rhs << '\n';
            continue;
        }
        throw TranslationError(line, "不支持的 Zig Intel 指令: " + mnemonic + " (" + sourceName + ")");
    }
    return translate(lowered.str(), sourceName);
}
}
