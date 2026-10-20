#include "Translator.h"

#include <fstream>
#include <iostream>
#include <iterator>

int main(int argc, char** argv)
{
    if (argc != 3)
    {
        std::cerr << "用法: myvm-translate <source.asm> <output.myvm.asm>\n";
        return 2;
    }
    std::ifstream input(argv[1]);
    if (!input) { std::cerr << "无法读取源文件\n"; return 1; }
    const std::string source((std::istreambuf_iterator<char>(input)), {});
    try
    {
        const auto output = myvm::translator::Translator::translate(source, argv[1]);
        std::ofstream file(argv[2], std::ios::trunc);
        if (!file || !(file << output)) { std::cerr << "无法写入目标文件\n"; return 1; }
    }
    catch (const myvm::translator::TranslationError& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    return 0;
}
