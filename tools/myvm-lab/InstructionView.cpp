#include "InstructionView.h"

#include <algorithm>
#include <array>
#include <iomanip>
#include <sstream>

namespace myvm::lab
{
namespace
{
std::uint64_t readLittleEndian(const std::vector<std::uint8_t>& bytes, std::size_t offset, std::size_t width)
{
    std::uint64_t value = 0;
    for (std::size_t i = 0; i < width; ++i)
        value |= static_cast<std::uint64_t>(bytes[offset + i]) << (i * 8);
    return value;
}

std::string number(std::uint64_t value)
{
    std::ostringstream output;
    output << "0x" << std::hex << std::uppercase << value;
    return output.str();
}

struct Definition
{
    const char* mnemonic;
    const char* meaning;
};

constexpr std::array<Definition, 23> definitions = {{
    {"rupt", "触发内部中断"}, {"leareg", "取得通用寄存器地址"},
    {"leamem", "读取后续内存槽地址"}, {"ld", "从地址暂存器读取 8 字节"},
    {"ldl", "设置数据暂存器低 32 位"}, {"ldh", "设置数据暂存器高 32 位"},
    {"save", "将数据暂存器写入地址暂存器"}, {"add", "将数据暂存器加到目标值"},
    {"dec", "从目标值减去数据暂存器"}, {"mul", "目标值乘以数据暂存器"},
    {"div", "目标值除以数据暂存器"}, {"cmp", "比较数据暂存器与目标值"},
    {"pause", "暂停指令"}, {"step", "增加 VM 步数"}, {"jmp", "跳转到数据暂存器地址"},
    {"asjmp", "比较标志匹配时跳转"}, {"push", "压入数据暂存器"},
    {"pop", "从栈弹出到数据暂存器"}, {"read", "从端口读取数据"},
    {"write", "向端口写入数据"}, {"call", "压入返回地址并调用"},
    {"ret", "从栈恢复返回地址"}, {"iret", "中断返回"}
}};
}

std::vector<InstructionRow> decodeProgram(const std::vector<std::uint8_t>& bytecode)
{
    std::vector<InstructionRow> rows;
    std::size_t offset = 0;
    while (offset < bytecode.size())
    {
        InstructionRow row;
        row.offset = offset;
        row.size = std::min<std::size_t>(8, bytecode.size() - offset);
        row.opcode = bytecode[offset];
        if (row.size < 8)
        {
            row.mnemonic = "数据片段";
            row.operands = std::to_string(row.size) + " 字节";
            row.meaning = "不足 8 字节，无法解码为完整指令";
            rows.push_back(std::move(row));
            break;
        }

        if (row.opcode >= definitions.size())
        {
            row.mnemonic = "未知指令";
            row.operands = number(row.opcode);
            row.meaning = "解释器会将未知操作码视为指令错误";
            rows.push_back(std::move(row));
            offset += row.size;
            continue;
        }

        row.known = true;
        row.mnemonic = definitions[row.opcode].mnemonic;
        row.meaning = definitions[row.opcode].meaning;
        switch (row.opcode)
        {
        case 0:
            row.operands = number(readLittleEndian(bytecode, offset + 4, 4));
            break;
        case 1:
            row.operands = "r" + std::to_string(readLittleEndian(bytecode, offset + 2, 4)) +
                ", t" + std::to_string(readLittleEndian(bytecode, offset + 6, 2));
            break;
        case 2:
            row.operands = "t" + std::to_string(readLittleEndian(bytecode, offset + 6, 2));
            if (bytecode.size() - offset >= 16)
            {
                row.size = 16;
                row.hasAddressLiteral = true;
                row.addressLiteral = readLittleEndian(bytecode, offset + 8, 8);
                row.meaning += "；后续槽位的索引为 " + number(row.addressLiteral);
            }
            else
                row.meaning += "；缺少后续地址字面量";
            break;
        case 3:
        case 6: case 7: case 8: case 9: case 10: case 11:
            row.operands = "t" + std::to_string(readLittleEndian(bytecode, offset + 6, 2));
            break;
        case 4:
        case 5:
            row.operands = number(readLittleEndian(bytecode, offset + 4, 4));
            break;
        case 15:
            row.operands = number(bytecode[offset + 7]);
            break;
        case 18:
        case 19:
            row.operands = number(readLittleEndian(bytecode, offset + 4, 4));
            break;
        default:
            break;
        }
        rows.push_back(std::move(row));
        offset += rows.back().size;
    }
    return rows;
}
}
