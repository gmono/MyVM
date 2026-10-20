#pragma once

#include <cstddef>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace myvm::assembler
{
class AssemblyError final : public std::runtime_error
{
public:
    AssemblyError(std::string source, std::size_t line, std::size_t column, std::string message);

    const std::string& source() const noexcept { return source_; }
    std::size_t line() const noexcept { return line_; }
    std::size_t column() const noexcept { return column_; }

private:
    std::string source_;
    std::size_t line_;
    std::size_t column_;
};

class Assembler final
{
public:
    static std::vector<std::uint8_t> assemble(
        const std::string& source,
        const std::string& sourceName = "<memory>");
};
}
