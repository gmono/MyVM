#include "X86Decoder.h"

#include <iostream>
#include <stdexcept>

int main()
{
    try
    {
        const std::vector<std::uint8_t> bytes = {
            0x48, 0x01, 0xF0,       // add rax, rsi
            0x48, 0x8D, 0x04, 0x37, // lea rax, [rdi + rsi]
            0xC3                    // ret
        };
        const auto instructions = myvm::x86::Decoder::decode(bytes, 0x1000);
        if (instructions.size() != 3 || instructions[0].mnemonic != "add" ||
            instructions[1].mnemonic != "lea" || instructions[2].mnemonic != "ret")
            throw std::runtime_error("x86 decoder golden output mismatch");
        if (instructions[1].operands.size() != 2 || instructions[1].operands[1].kind != "memory")
            throw std::runtime_error("x86 memory operand metadata missing");
        std::cout << "x86 decoder tests passed\n";
        return 0;
    }
    catch (const std::exception& error) { std::cerr << error.what() << '\n'; return 1; }
}
