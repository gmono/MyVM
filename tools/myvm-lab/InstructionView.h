#pragma once

#include "mvm.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace myvm::lab
{
struct InstructionRow
{
    std::size_t offset = 0;
    std::size_t size = 0;
    std::uint8_t opcode = 0;
    std::string mnemonic;
    std::string operands;
    std::string meaning;
    bool known = false;
    bool hasAddressLiteral = false;
    qword addressLiteral = 0;
};

std::vector<InstructionRow> decodeProgram(const std::vector<std::uint8_t>& bytecode);
}
