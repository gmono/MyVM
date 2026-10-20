#include "LabSession.h"

#include "Assembler.h"

#include <algorithm>
#include <limits>
#include <stdexcept>

namespace myvm::lab
{
bool LabSession::assemble(const std::string& source, AssemblyDiagnostic& diagnostic)
{
    try
    {
        const auto assembled = assembler::Assembler::assemble(source, "<editor>");
        return createVm(assembled, diagnostic);
    }
    catch (const assembler::AssemblyError& error)
    {
        diagnostic.source = error.source();
        diagnostic.message = error.what();
        diagnostic.line = error.line();
        diagnostic.column = error.column();
        return false;
    }
    catch (const std::exception& error)
    {
        setFailure(diagnostic, error.what());
        return false;
    }
}

bool LabSession::load(const std::vector<std::uint8_t>& bytecode, AssemblyDiagnostic& diagnostic)
{
    try
    {
        return createVm(bytecode, diagnostic);
    }
    catch (const std::exception& error)
    {
        setFailure(diagnostic, error.what());
        return false;
    }
}

bool LabSession::reset(AssemblyDiagnostic& diagnostic)
{
    if (bytecode_.empty() && !vm_)
    {
        setFailure(diagnostic, "没有可复位的程序");
        return false;
    }
    return createVm(bytecode_, diagnostic);
}

bool LabSession::step(AssemblyDiagnostic& diagnostic)
{
    if (!vm_)
    {
        setFailure(diagnostic, "请先汇编或加载程序");
        return false;
    }

    try
    {
        const auto before = vm_->inspect();
        const auto pc = static_cast<std::size_t>(before.state.regs[0]);
        const auto opcode = pc < memory_.size() ? memory_[pc] : byte{0xff};
        if (!vm_->run(1))
        {
            setFailure(diagnostic, "VM 当前不可执行单步");
            return false;
        }
        const auto after = vm_->inspect();
        const qword sequence = trace_.empty() ? 1 : trace_.back().sequence + 1;
        trace_.push_back({before.state.regs[0], after.state.regs[0], before.currdata,
                          after.currdata, sequence, after.state.nowstep, opcode});
        if (trace_.size() > 4096) trace_.erase(trace_.begin());
        clearDiagnostic(diagnostic);
        return true;
    }
    catch (const std::exception& error)
    {
        setFailure(diagnostic, error.what());
        return false;
    }
}

std::size_t LabSession::run(std::size_t maxSteps, AssemblyDiagnostic& diagnostic)
{
    std::size_t completed = 0;
    clearDiagnostic(diagnostic);
    while (completed < maxSteps)
    {
        if (!step(diagnostic)) break;
        ++completed;
        if (!trace_.empty() && trace_.back().opcode == 0x0c) break;
    }
    return completed;
}

MVMInspection LabSession::inspect() const
{
    if (!vm_) throw std::logic_error("请先汇编或加载程序");
    return vm_->inspect();
}

bool LabSession::createVm(const std::vector<std::uint8_t>& bytecode, AssemblyDiagnostic& diagnostic)
{
    constexpr std::size_t stackSize = 4096;
    if (bytecode.size() > std::numeric_limits<std::size_t>::max() - 7 - stackSize)
    {
        setFailure(diagnostic, "程序过大");
        return false;
    }

    const auto alignedCodeSize = (bytecode.size() + 7) & ~std::size_t{7};
    std::vector<byte> candidateMemory(alignedCodeSize + stackSize, 0);
    std::copy(bytecode.begin(), bytecode.end(), candidateMemory.begin());

    VMState state;
    state.regs = Array<qword, dword>(8);
    state.regs[3] = reinterpret_cast<qword>(candidateMemory.data() + candidateMemory.size());
    state.programptr = candidateMemory.data();
    state.len = candidateMemory.size();

    auto candidateVm = std::make_unique<MVM>(state);
    memory_ = std::move(candidateMemory);
    bytecode_ = bytecode;
    vm_ = std::move(candidateVm);
    trace_.clear();
    clearDiagnostic(diagnostic);
    return true;
}

void LabSession::clearDiagnostic(AssemblyDiagnostic& diagnostic) const
{
    diagnostic = {};
}

void LabSession::setFailure(AssemblyDiagnostic& diagnostic, const std::string& message) const
{
    diagnostic.source = "VM";
    diagnostic.message = message;
    diagnostic.line = 0;
    diagnostic.column = 0;
}
}
