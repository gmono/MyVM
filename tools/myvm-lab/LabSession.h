#pragma once

#include "mvm.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace myvm::lab
{
struct AssemblyDiagnostic
{
    std::string source;
    std::string message;
    std::size_t line = 0;
    std::size_t column = 0;
};

struct TraceEntry
{
    qword pcBefore = 0;
    qword pcAfter = 0;
    qword dataBefore = 0;
    qword dataAfter = 0;
    qword sequence = 0;
    qword vmStep = 0;
    byte opcode = 0;
};

class LabSession final
{
public:
    bool assemble(const std::string& source, AssemblyDiagnostic& diagnostic);
    bool load(const std::vector<std::uint8_t>& bytecode, AssemblyDiagnostic& diagnostic);
    bool reset(AssemblyDiagnostic& diagnostic);
    bool step(AssemblyDiagnostic& diagnostic);
    std::size_t run(std::size_t maxSteps, AssemblyDiagnostic& diagnostic);

    bool hasProgram() const noexcept { return static_cast<bool>(vm_); }
    std::size_t codeSize() const noexcept { return bytecode_.size(); }
    const std::vector<std::uint8_t>& bytecode() const noexcept { return bytecode_; }
    const std::vector<byte>& memory() const noexcept { return memory_; }
    const std::vector<TraceEntry>& trace() const noexcept { return trace_; }
    MVMInspection inspect() const;

private:
    bool createVm(const std::vector<std::uint8_t>& bytecode, AssemblyDiagnostic& diagnostic);
    void clearDiagnostic(AssemblyDiagnostic& diagnostic) const;
    void setFailure(AssemblyDiagnostic& diagnostic, const std::string& message) const;

    std::vector<std::uint8_t> bytecode_;
    std::vector<byte> memory_;
    std::unique_ptr<MVM> vm_;
    std::vector<TraceEntry> trace_;
};
}
