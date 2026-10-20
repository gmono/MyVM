#include "Assembler.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <iterator>
#include <chrono>
#include <random>
#include <string>
#include <vector>

namespace
{
void printUsage(std::ostream& output)
{
    output << "Usage: myvm-asm <input.asm> -o <output.bin>\n"
              "       myvm-asm --help\n";
}

void writeFileAtomically(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes)
{
    std::random_device random;
    const auto nonce = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count()) +
        "." + std::to_string(random());
    const auto temporary = std::filesystem::path(path.string() + ".tmp." + nonce);
    const auto backup = std::filesystem::path(path.string() + ".bak." + nonce);
    try
    {
        {
            std::ofstream output(temporary, std::ios::binary | std::ios::trunc);
            if (!output) throw std::runtime_error("cannot open temporary output file: " + temporary.string());
            output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
            if (!output) throw std::runtime_error("failed while writing output file: " + temporary.string());
        }
        std::error_code error;
        std::filesystem::rename(temporary, path, error);
        if (error)
        {
            if (!std::filesystem::exists(path))
                throw std::runtime_error("cannot replace output file: " + error.message());

            error.clear();
            std::filesystem::rename(path, backup, error);
            if (error) throw std::runtime_error("cannot preserve existing output file: " + error.message());

            std::filesystem::rename(temporary, path, error);
            if (error)
            {
                const auto replaceError = error.message();
                error.clear();
                std::filesystem::rename(backup, path, error);
                if (error) throw std::runtime_error("output replacement failed and backup restore failed: " + error.message());
                throw std::runtime_error("cannot replace output file: " + replaceError);
            }
            std::filesystem::remove(backup, error);
        }
    }
    catch (...)
    {
        std::error_code ignored;
        std::filesystem::remove(temporary, ignored);
        throw;
    }
}
}

int main(int argc, char** argv)
{
    try
    {
        if (argc == 2 && (std::string(argv[1]) == "--help" || std::string(argv[1]) == "-h"))
        {
            printUsage(std::cout);
            return 0;
        }
        if (argc != 4 || std::string(argv[2]) != "-o")
        {
            printUsage(std::cerr);
            return 2;
        }

        const std::filesystem::path inputPath(argv[1]);
        const std::filesystem::path outputPath(argv[3]);
        std::ifstream input(inputPath, std::ios::binary);
        if (!input) throw std::runtime_error("cannot open input file: " + inputPath.string());
        const std::string source(std::istreambuf_iterator<char>(input), {});
        const auto bytes = myvm::assembler::Assembler::assemble(source, inputPath.string());
        if (outputPath.has_parent_path()) std::filesystem::create_directories(outputPath.parent_path());
        writeFileAtomically(outputPath, bytes);
        std::cout << "Assembled " << bytes.size() << " bytes into " << outputPath.string() << '\n';
        return 0;
    }
    catch (const myvm::assembler::AssemblyError& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
