#include "Translator.h"

#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc, char** argv)
{
    const bool zigIntel = argc == 4 && std::string(argv[1]) == "--zig-intel";
    if ((!zigIntel && argc != 3) || (zigIntel && argc != 4))
    {
        std::cerr << "用法: myvm-translate <source.asm> <output.myvm.asm>\n"
                     "   或: myvm-translate --zig-intel <zig.s> <output.myvm.asm>\n";
        return 2;
    }
    const char* inputPath = zigIntel ? argv[2] : argv[1];
    std::ifstream input(inputPath);
    if (!input) { std::cerr << "无法读取源文件\n"; return 1; }
    const std::string source((std::istreambuf_iterator<char>(input)), {});
    try
    {
        const auto output = zigIntel
            ? myvm::translator::Translator::translateZigIntel(source, argv[2])
            : myvm::translator::Translator::translate(source, argv[1]);
        std::ofstream file(zigIntel ? argv[3] : argv[2], std::ios::trunc);
        if (!file || !(file << output)) { std::cerr << "无法写入目标文件\n"; return 1; }
    }
    catch (const myvm::translator::TranslationError& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
