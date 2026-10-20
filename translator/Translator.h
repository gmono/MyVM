#pragma once

#include <cstddef>
#include <stdexcept>
#include <string>

namespace myvm::translator
{
class TranslationError final : public std::runtime_error
{
public:
    TranslationError(std::size_t line, std::string message);
    std::size_t line() const noexcept { return line_; }

private:
    std::size_t line_;
};

class Translator final
{
public:
    static std::string translate(const std::string& source,
                                 const std::string& sourceName = "<source>");
};
}
