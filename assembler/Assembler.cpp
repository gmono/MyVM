#include "Assembler.h"

#include "MyVMAsmLexer.h"
#include "MyVMAsmParser.h"
#include "antlr4-runtime.h"

#include <algorithm>
#include <array>
#include <charconv>
#include <cctype>
#include <cstdint>
#include <functional>
#include <limits>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>

namespace myvm::assembler
{
namespace
{
enum class Section { none, symbols, code, data };

struct LocatedStatement
{
    MyVMAsmParser::StatementContext* statement;
    Section section;
};

struct Instruction
{
    std::uint8_t opcode;
    std::vector<std::size_t> widths;
    enum class IndexOperand { none, registerId, temporaryId } indexOperand = IndexOperand::none;
};

const std::unordered_map<std::string, Instruction>& instructionSet()
{
    static const std::unordered_map<std::string, Instruction> instructions = []
    {
        std::unordered_map<std::string, Instruction> result;
        const auto add = [&](std::string name, std::uint8_t opcode, std::vector<std::size_t> widths,
                             Instruction::IndexOperand index = Instruction::IndexOperand::none)
        {
            result.emplace(std::move(name), Instruction{opcode, std::move(widths), index});
        };
        using Index = Instruction::IndexOperand;
        add("rupt", 0, {4}); add("leareg", 1, {4, 2}, Index::registerId);
        add("leamem", 2, {2}, Index::temporaryId); add("ld", 3, {2}, Index::temporaryId);
        add("ldl", 4, {4}); add("ldh", 5, {4}); add("save", 6, {2}, Index::temporaryId);
        add("add", 7, {2}, Index::temporaryId); add("dec", 8, {2}, Index::temporaryId);
        add("mul", 9, {2}, Index::temporaryId); add("div", 10, {2}, Index::temporaryId);
        add("cmp", 11, {2}, Index::temporaryId); add("pause", 12, {}); add("step", 13, {});
        add("jmp", 14, {}); add("asjmp", 15, {1}); add("push", 16, {}); add("pop", 17, {});
        add("read", 18, {4}); add("write", 19, {4}); add("call", 20, {});
        add("ret", 21, {}); add("iret", 22, {});
        return result;
    }();
    return instructions;
}

class ErrorCollector final : public antlr4::BaseErrorListener
{
public:
    struct Item { std::size_t line; std::size_t column; std::string message; };

    void syntaxError(antlr4::Recognizer*, antlr4::Token*, std::size_t line,
                     std::size_t column, const std::string& message,
                     std::exception_ptr) override
    {
        errors.push_back({line, column + 1, message});
    }

    std::vector<Item> errors;
};

class Engine
{
public:
    Engine(const std::string& input, const std::string& sourceName)
        : sourceName_(sourceName), input_(input), lexer_(&input_), tokens_(&lexer_), parser_(&tokens_)
    {
        lexer_.removeErrorListeners();
        lexer_.addErrorListener(&errors_);
        parser_.removeErrorListeners();
        parser_.addErrorListener(&errors_);
    }

    std::vector<std::uint8_t> assemble()
    {
        auto* tree = parser_.program();
        if (!errors_.errors.empty())
        {
            const auto& error = errors_.errors.front();
            throw AssemblyError(sourceName_, error.line, error.column, error.message);
        }

        collectDeclarations(tree);
        layout(tree);
        emit(tree);
        return output_;
    }

private:
    std::string sourceName_;
    antlr4::ANTLRInputStream input_;
    MyVMAsmLexer lexer_;
    antlr4::CommonTokenStream tokens_;
    MyVMAsmParser parser_;
    ErrorCollector errors_;
    Section section_ = Section::none;
    std::vector<LocatedStatement> statements_;
    std::unordered_map<std::string, MyVMAsmParser::ExpressionContext*> constants_;
    std::unordered_map<std::string, std::uint64_t> constantValues_;
    std::unordered_map<std::string, std::uint64_t> labels_;
    std::unordered_set<std::string> resolvingConstants_;
    std::vector<std::uint8_t> output_;
    std::uint64_t cursor_ = 0;

    [[noreturn]] void fail(const antlr4::ParserRuleContext* context, const std::string& message) const
    {
        const auto* token = context ? context->getStart() : nullptr;
        throw AssemblyError(sourceName_, token ? token->getLine() : 1,
                            token ? token->getCharPositionInLine() + 1 : 1, message);
    }

    void collectDeclarations(MyVMAsmParser::ProgramContext* tree)
    {
        for (auto* statement : tree->statement())
        {
            if (auto* directive = statement->sectionDirective())
            {
                if (directive->HASH_SYMBOL()) section_ = Section::symbols;
                else if (directive->HASH_CODE() || directive->DOT_CODE()) section_ = Section::code;
                else if (directive->DOT_DATA()) section_ = Section::data;
                else section_ = Section::none;
                continue;
            }

            if (auto* definition = statement->constantDefinition())
            {
                addConstant(definition->IDENT()->getText(), definition->expression(), definition);
                continue;
            }

            if (auto* definition = statement->legacyConstantDefinition())
            {
                if (section_ == Section::symbols)
                {
                    addConstant(definition->IDENT()->getText(), definition->expression(), definition);
                    continue;
                }
                if (section_ == Section::code || section_ == Section::data)
                    fail(definition, "use a label without a value, or declare a constant with .const");
            }

            if (statement->labelDefinition())
            {
                if (section_ != Section::code && section_ != Section::data)
                    fail(statement, "labels must be declared inside .code or .data");
                statements_.push_back({statement, section_});
                continue;
            }

            if (statement->instruction() || statement->dataDirective())
            {
                if (section_ != Section::code && section_ != Section::data)
                    fail(statement, "instructions and data directives require .code or .data");
                statements_.push_back({statement, section_});
            }
        }
    }

    void addConstant(const std::string& name, MyVMAsmParser::ExpressionContext* expression,
                     antlr4::ParserRuleContext* context)
    {
        if (constants_.find(name) != constants_.end()) fail(context, "duplicate constant '" + name + "'");
        constants_.emplace(name, expression);
    }

    void layout(MyVMAsmParser::ProgramContext*)
    {
        for (const auto& item : statements_)
        {
            if (auto* label = item.statement->labelDefinition())
            {
                const auto name = label->IDENT()->getText();
                if (constants_.find(name) != constants_.end() || !labels_.emplace(name, cursor_).second)
                    fail(label, "duplicate symbol '" + name + "'");
                continue;
            }

            const auto size = statementSize(item);
            if (size > std::numeric_limits<std::uint64_t>::max() - cursor_)
                fail(item.statement, "assembled output is too large");
            cursor_ += size;
        }
        if (cursor_ > output_.max_size()) throw std::length_error("assembled output is too large");
    }

    std::uint64_t statementSize(const LocatedStatement& item)
    {
        if (auto* instruction = item.statement->instruction())
        {
            if (item.section != Section::code) fail(instruction, "machine instructions are only valid in .code");
            const auto name = lower(instruction->IDENT()->getText());
            if (name == "num")
            {
                const auto operands = getOperands(instruction->operandList(), instruction);
                requireOperandCount(operands, 2, instruction, "num");
                const auto width = evaluate(operands[0]->expression());
                if (width < 1 || width > 8) fail(operands[0], "num width must be between 1 and 8 bytes");
                return width;
            }
            if (name == "data")
            {
                const auto operands = getOperands(instruction->operandList(), instruction);
                if (operands.empty()) fail(instruction, "data expects one or more byte values");
                return operands.size();
            }
            const auto found = instructionSet().find(name);
            if (found == instructionSet().end()) fail(instruction, "unknown instruction '" + name + "'");
            const auto operands = getOperands(instruction->operandList(), instruction);
            requireOperandCount(operands, found->second.widths.size(), instruction, name);
            return 8;
        }

        auto* directive = item.statement->dataDirective();
        if (!directive) fail(item.statement, "invalid statement");
        if (item.section != Section::data) fail(directive, "data directives are only valid in .data");
        if (directive->DOT_ASCII()) return decodeString(directive->STRING()->getText(), directive).size();

        const auto operands = getOperands(directive->operandList(), directive);
        if (directive->DOT_SPACE())
        {
            requireOperandCount(operands, 1, directive, ".space");
            return evaluate(operands[0]->expression());
        }
        if (directive->DOT_ALIGN())
        {
            requireOperandCount(operands, 1, directive, ".align");
            const auto alignment = evaluate(operands[0]->expression());
            if (alignment == 0 || alignment > 4096 || (alignment & (alignment - 1)) != 0)
                fail(operands[0], ".align requires a power of two between 1 and 4096");
            return (alignment - cursor_ % alignment) % alignment;
        }
        if (operands.empty()) fail(directive, "data directive requires at least one value");
        return operands.size() * directiveWidth(directive);
    }

    void emit(MyVMAsmParser::ProgramContext*)
    {
        output_.clear();
        cursor_ = 0;
        for (const auto& item : statements_)
        {
            if (item.statement->labelDefinition()) continue;
            if (auto* instruction = item.statement->instruction()) emitInstruction(instruction);
            else emitDirective(item.statement->dataDirective());
        }
    }

    void emitInstruction(MyVMAsmParser::InstructionContext* instruction)
    {
        const auto name = lower(instruction->IDENT()->getText());
        const auto operands = getOperands(instruction->operandList(), instruction);
        if (name == "num")
        {
            const auto width = evaluate(operands[0]->expression());
            const auto value = evaluate(operands[1]->expression());
            appendInteger(value, static_cast<std::size_t>(width), operands[1]);
            return;
        }
        if (name == "data")
        {
            for (auto* operand : operands) appendInteger(evaluate(operand->expression()), 1, operand);
            return;
        }

        const auto& instructionInfo = instructionSet().at(name);
        std::array<std::uint8_t, 8> bytes{};
        bytes[0] = instructionInfo.opcode;
        auto offset = 8 - std::accumulate(instructionInfo.widths.begin(), instructionInfo.widths.end(), std::size_t{0});
        for (std::size_t i = 0; i < operands.size(); ++i)
        {
            const auto value = indexOperand(instructionInfo, i, operands[i]);
            ensureFits(value, instructionInfo.widths[i], operands[i]);
            writeInteger(bytes.data() + offset, value, instructionInfo.widths[i]);
            offset += instructionInfo.widths[i];
        }
        output_.insert(output_.end(), bytes.begin(), bytes.end());
        cursor_ += bytes.size();
    }

    std::uint64_t indexOperand(const Instruction& instruction, std::size_t index,
                               MyVMAsmParser::OperandContext* operand)
    {
        const auto text = operand->expression()->getText();
        const auto kind = instruction.indexOperand;
        std::string prefix;
        if (kind == Instruction::IndexOperand::registerId && index == 0) prefix = "r";
        else if (kind == Instruction::IndexOperand::temporaryId ||
                 (kind == Instruction::IndexOperand::registerId && index == 1)) prefix = "t";

        if (!prefix.empty() && text.size() > 1 && lower(text.substr(0, 1)) == prefix &&
            std::all_of(text.begin() + 1, text.end(), [](unsigned char ch) { return std::isdigit(ch) != 0; }))
        {
            std::uint64_t value = 0;
            const auto result = std::from_chars(text.data() + 1, text.data() + text.size(), value, 10);
            if (result.ec != std::errc{}) fail(operand, "register index is out of range");
            return value;
        }
        return evaluate(operand->expression());
    }

    void emitDirective(MyVMAsmParser::DataDirectiveContext* directive)
    {
        if (directive->DOT_ASCII())
        {
            auto bytes = decodeString(directive->STRING()->getText(), directive);
            output_.insert(output_.end(), bytes.begin(), bytes.end());
            cursor_ += bytes.size();
            return;
        }

        const auto operands = getOperands(directive->operandList(), directive);
        if (directive->DOT_SPACE())
        {
            const auto count = evaluate(operands[0]->expression());
            output_.insert(output_.end(), static_cast<std::size_t>(count), 0);
            cursor_ += count;
            return;
        }
        if (directive->DOT_ALIGN())
        {
            const auto alignment = evaluate(operands[0]->expression());
            const auto count = (alignment - cursor_ % alignment) % alignment;
            output_.insert(output_.end(), static_cast<std::size_t>(count), 0);
            cursor_ += count;
            return;
        }

        const auto width = directiveWidth(directive);
        for (auto* operand : operands) appendInteger(evaluate(operand->expression()), width, operand);
    }

    void appendInteger(std::uint64_t value, std::size_t width, antlr4::ParserRuleContext* context)
    {
        ensureFits(value, width, context);
        const auto start = output_.size();
        output_.resize(start + width);
        writeInteger(output_.data() + start, value, width);
        cursor_ += width;
    }

    void ensureFits(std::uint64_t value, std::size_t width, antlr4::ParserRuleContext* context) const
    {
        if (width == 0 || width > 8) fail(context, "integer width must be between 1 and 8 bytes");
        if (width < 8 && value >= (std::uint64_t{1} << (width * 8)))
            fail(context, "value does not fit in the requested byte width");
    }

    static void writeInteger(std::uint8_t* destination, std::uint64_t value, std::size_t width)
    {
        for (std::size_t index = 0; index < width; ++index)
            destination[index] = static_cast<std::uint8_t>(value >> (index * 8));
    }

    std::uint64_t evaluate(MyVMAsmParser::ExpressionContext* expression)
    {
        return evaluate(expression->bitwiseOr());
    }

    std::uint64_t evaluate(MyVMAsmParser::BitwiseOrContext* context)
    {
        return fold(context->bitwiseXor(), context, [](auto left, auto right, const auto&) { return left | right; });
    }
    std::uint64_t evaluate(MyVMAsmParser::BitwiseXorContext* context)
    {
        return fold(context->bitwiseAnd(), context, [](auto left, auto right, const auto&) { return left ^ right; });
    }
    std::uint64_t evaluate(MyVMAsmParser::BitwiseAndContext* context)
    {
        return fold(context->shiftExpr(), context, [](auto left, auto right, const auto&) { return left & right; });
    }

    template<typename Terms, typename Context, typename Operation>
    std::uint64_t fold(const Terms& terms, Context* context, Operation operation)
    {
        auto value = evaluate(terms[0]);
        for (std::size_t index = 1; index < terms.size(); ++index)
        {
            const auto op = context->children[index * 2 - 1]->getText();
            value = operation(value, evaluate(terms[index]), op);
        }
        return value;
    }

    std::uint64_t evaluate(MyVMAsmParser::ShiftExprContext* context)
    {
        auto values = context->additiveExpr();
        auto value = evaluate(values[0]);
        for (std::size_t index = 1; index < values.size(); ++index)
        {
            const auto count = evaluate(values[index]);
            if (count > 63) fail(context, "shift count must be between 0 and 63");
            const auto op = context->children[index * 2 - 1]->getText();
            value = op == "<<" ? value << count : value >> count;
        }
        return value;
    }

    std::uint64_t evaluate(MyVMAsmParser::AdditiveExprContext* context)
    {
        auto values = context->multiplicativeExpr();
        auto value = evaluate(values[0]);
        for (std::size_t index = 1; index < values.size(); ++index)
        {
            const auto rhs = evaluate(values[index]);
            const auto op = context->children[index * 2 - 1]->getText();
            if (op == "+")
            {
                if (rhs > std::numeric_limits<std::uint64_t>::max() - value) fail(context, "integer expression overflow");
                value += rhs;
            }
            else
            {
                if (rhs > value) fail(context, "unsigned integer expression underflow");
                value -= rhs;
            }
        }
        return value;
    }

    std::uint64_t evaluate(MyVMAsmParser::MultiplicativeExprContext* context)
    {
        auto values = context->unaryExpr();
        auto value = evaluate(values[0]);
        for (std::size_t index = 1; index < values.size(); ++index)
        {
            const auto rhs = evaluate(values[index]);
            const auto op = context->children[index * 2 - 1]->getText();
            if (op == "*")
            {
                if (value && rhs > std::numeric_limits<std::uint64_t>::max() / value)
                    fail(context, "integer expression overflow");
                value *= rhs;
            }
            else
            {
                if (rhs == 0) fail(context, "division by zero in expression");
                value = op == "/" ? value / rhs : value % rhs;
            }
        }
        return value;
    }

    std::uint64_t evaluate(MyVMAsmParser::UnaryExprContext* context)
    {
        auto value = evaluate(context->primary());
        for (auto it = context->children.rbegin() + 1; it != context->children.rend(); ++it)
        {
            const auto op = (*it)->getText();
            if (op == "~") value = ~value;
            else if (op == "-")
            {
                if (value != 0) fail(context, "negative values cannot be encoded in the unsigned VM format");
            }
        }
        return value;
    }

    std::uint64_t evaluate(MyVMAsmParser::PrimaryContext* context)
    {
        if (context->expression()) return evaluate(context->expression());
        if (context->NUMBER()) return parseNumber(context->NUMBER()->getText(), context);
        const auto name = context->IDENT()->getText();
        const bool explicitSymbol = context->DOLLAR() != nullptr;
        const auto constant = constants_.find(name);
        if (constant != constants_.end())
        {
            const auto cached = constantValues_.find(name);
            if (cached != constantValues_.end()) return cached->second;
            if (!resolvingConstants_.insert(name).second) fail(context, "cyclic constant definition involving '" + name + "'");
            const auto value = evaluate(constant->second);
            resolvingConstants_.erase(name);
            constantValues_.emplace(name, value);
            return value;
        }
        const auto label = labels_.find(name);
        if (label != labels_.end()) return label->second;
        if (!explicitSymbol && isLegacyHex(name)) return parseNumber(name, context);
        fail(context, "undefined symbol '" + name + "'");
    }

    std::uint64_t parseNumber(const std::string& token, antlr4::ParserRuleContext* context) const
    {
        std::string text;
        for (const auto ch : token) if (ch != '_') text.push_back(ch);
        int base = 16;
        const char* begin = text.data();
        if (text.size() > 1 && (text[0] == 'd' || text[0] == 'D')) { base = 10; ++begin; }
        else if (text.size() > 2 && text[0] == '0' && (text[1] == 'x' || text[1] == 'X')) { base = 16; begin += 2; }
        else if (text.size() > 2 && text[0] == '0' && (text[1] == 'b' || text[1] == 'B')) { base = 2; begin += 2; }
        else if (text.size() > 2 && text[0] == '0' && (text[1] == 'o' || text[1] == 'O')) { base = 8; begin += 2; }
        std::uint64_t value = 0;
        const auto result = std::from_chars(begin, text.data() + text.size(), value, base);
        if (begin == text.data() + text.size() || result.ec != std::errc{} || result.ptr != text.data() + text.size())
            fail(context, "invalid or out-of-range integer '" + token + "'");
        return value;
    }

    static bool isLegacyHex(const std::string& text)
    {
        return !text.empty() && std::all_of(text.begin(), text.end(), [](unsigned char ch)
        {
            return std::isxdigit(ch) != 0;
        });
    }

    static std::string lower(std::string text)
    {
        std::transform(text.begin(), text.end(), text.begin(), [](unsigned char ch)
        {
            return static_cast<char>(std::tolower(ch));
        });
        return text;
    }

    std::vector<MyVMAsmParser::OperandContext*> getOperands(
        MyVMAsmParser::OperandListContext* list, antlr4::ParserRuleContext* owner) const
    {
        if (!list) return {};
        return list->operand();
    }

    void requireOperandCount(const std::vector<MyVMAsmParser::OperandContext*>& operands,
                             std::size_t expected, antlr4::ParserRuleContext* context,
                             const std::string& name) const
    {
        if (operands.size() != expected)
            fail(context, "'" + name + "' expects " + std::to_string(expected) + " operand(s), got " +
                          std::to_string(operands.size()));
    }

    std::size_t directiveWidth(MyVMAsmParser::DataDirectiveContext* directive) const
    {
        if (directive->DOT_BYTE()) return 1;
        if (directive->DOT_WORD()) return 2;
        if (directive->DOT_DWORD()) return 4;
        if (directive->DOT_QWORD()) return 8;
        fail(directive, "unknown data directive");
    }

    std::vector<std::uint8_t> decodeString(const std::string& text, antlr4::ParserRuleContext* context) const
    {
        std::vector<std::uint8_t> bytes;
        for (std::size_t i = 1; i + 1 < text.size(); ++i)
        {
            auto ch = text[i];
            if (ch == '\\')
            {
                if (++i + 1 >= text.size()) fail(context, "unterminated string escape");
                switch (text[i])
                {
                    case 'n': ch = '\n'; break;
                    case 'r': ch = '\r'; break;
                    case 't': ch = '\t'; break;
                    case '0': ch = '\0'; break;
                    case '\\': ch = '\\'; break;
                    case '"': ch = '"'; break;
                    default: fail(context, "unsupported string escape");
                }
            }
            bytes.push_back(static_cast<std::uint8_t>(ch));
        }
        return bytes;
    }
};
}

AssemblyError::AssemblyError(std::string source, std::size_t line, std::size_t column, std::string message)
    : std::runtime_error(source + ":" + std::to_string(line) + ":" + std::to_string(column) + ": " + message),
      source_(std::move(source)), line_(line), column_(column)
{
}

std::vector<std::uint8_t> Assembler::assemble(const std::string& source, const std::string& sourceName)
{
    return Engine(source, sourceName).assemble();
}
}
