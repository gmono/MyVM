#include "Translator.h"

#include <iostream>
#include <stdexcept>

using myvm::translator::TranslationError;
using myvm::translator::Translator;

static void require(bool value, const char* message)
{
    if (!value) throw std::runtime_error(message);
}

int main()
{
    try
    {
        const auto output = Translator::translate(
            "; source dialect\n"
            "start:\n"
            "  mov r5, 42\n"
            "  add r5, 2\n"
            "  cmp r5, 44\n"
            "  je done\n"
            "  jmp start\n"
            "done:\n"
            "  push r5\n"
            "  pop r6\n"
            "  halt\n");
        require(output.find("ldl #d42") != std::string::npos, "decimal immediate was not preserved");
        require(output.find("add t0") != std::string::npos, "add lowering missing");
        require(output.find("cmp t0") != std::string::npos, "cmp lowering missing");
        require(output.find("asjmp #1") != std::string::npos, "conditional branch lowering missing");
        require(output.find("push") != std::string::npos && output.find("pop") != std::string::npos,
                "stack lowering missing");
        require(output.find("; @macro line=4 op=add") != std::string::npos,
                "macro source mapping missing");
        const auto bitOutput = Translator::translate("mov r5, 15\nand r5, 3\nshl r5, 1\n");
        require(bitOutput.find("and t0") != std::string::npos, "bitwise lowering missing");
        require(bitOutput.find("shl t0") != std::string::npos, "shift lowering missing");

        const auto zigOutput = Translator::translateZigIntel(
            ".intel_syntax noprefix\n"
            ".globl add\n"
            "add = .Lmain.add\n"
            ".Lmain.add:\n"
            "push rbp\n"
            "mov rbp, rsp\n"
            "lea rax, [rdi + rsi]\n"
            "pop rbp\n"
            "ret\n");
        require(zigOutput.find("add:") != std::string::npos, "Zig public alias missing");
        require(zigOutput.find("add t0") != std::string::npos, "Zig lea lowering missing");

        bool rejected = false;
        try { (void)Translator::translate("mov r5, 1\nwat r5\n"); }
        catch (const TranslationError& error) { rejected = error.line() == 2; }
        require(rejected, "unknown source instruction was not diagnosed");
        std::cout << "translator tests passed\n";
        return 0;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
