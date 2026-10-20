#include "Assembler.h"

#include <array>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using myvm::assembler::Assembler;
using myvm::assembler::AssemblyError;

namespace
{
void require(bool condition, const std::string& message)
{
    if (!condition) throw std::runtime_error(message);
}

void legacyEncodingIsPreserved()
{
    const std::string source = R"ASM(#symbol
target:ff
#end
#code
leareg 0 1
num d2 1234
leamem 2
num d8 $target
#end
)ASM";
    const std::vector<std::uint8_t> expected = {
        1, 0, 0, 0, 0, 0, 1, 0,
        0x34, 0x12,
        2, 0, 0, 0, 0, 0, 2, 0,
        0xff, 0, 0, 0, 0, 0, 0, 0
    };
    require(Assembler::assemble(source) == expected, "legacy instruction encoding changed");
}

void modernFormatSupportsLabelsExpressionsAndData()
{
    const std::string source = R"ASM(.const count = 2 + 1
.code
start:
    ldl #0x2a
    leareg r5, t0
    save t0
    jmp
.data
payload:
    .byte 1, d2, 0b11
    .word start + 8
    .ascii "vm\n"
    .space count
    .align 8
)ASM";
    const std::vector<std::uint8_t> expected = {
        4, 0, 0, 0, 0x2a, 0, 0, 0,
        1, 0, 5, 0, 0, 0, 0, 0,
        6, 0, 0, 0, 0, 0, 0, 0,
        14, 0, 0, 0, 0, 0, 0, 0,
        1, 2, 3, 8, 0, 'v', 'm', '\n',
        0, 0, 0, 0, 0, 0, 0, 0
    };
    require(Assembler::assemble(source, "modern.asm") == expected, "modern assembly output mismatch");
}

void rejectsUndefinedSymbolsWithSourceLocation()
{
    try
    {
        Assembler::assemble(".code\n  ldl #missing\n", "broken.asm");
    }
    catch (const AssemblyError& error)
    {
        require(error.source() == "broken.asm", "diagnostic source mismatch");
        require(error.line() == 2, "diagnostic line mismatch");
        require(error.column() >= 8, "diagnostic column is not precise");
        return;
    }
    throw std::runtime_error("undefined symbols must be rejected");
}

void rejectsDuplicateLabelsAndOverflow()
{
    std::size_t caseNumber = 0;
    for (const auto* source : {
        ".code\nentry:\nentry:\n",
        ".code\nldl #0x100000000\n",
        ".code\nnum d9 1\n"
    })
    {
        ++caseNumber;
        bool rejected = false;
        try { (void)Assembler::assemble(source); }
        catch (const AssemblyError&) { rejected = true; }
        require(rejected, "invalid assembly case " + std::to_string(caseNumber) + " was accepted");
    }
}

void keywordsAreCaseInsensitiveAndStringsAreUtf8()
{
    const auto bytes = Assembler::assemble(".DATA\n.ASCII \"中\"\n", "unicode.asm");
    require(bytes == std::vector<std::uint8_t>({0xe4, 0xb8, 0xad}), "string data must be UTF-8");
}
}

int main()
{
    try
    {
        legacyEncodingIsPreserved();
        modernFormatSupportsLabelsExpressionsAndData();
        rejectsUndefinedSymbolsWithSourceLocation();
        rejectsDuplicateLabelsAndOverflow();
        keywordsAreCaseInsensitiveAndStringsAreUtf8();
        std::cout << "5 assembler tests passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
