#include "mvm.h"
#include "InstructionView.h"
#include "LabSession.h"

#include <array>
#include <cstdlib>
#include <iostream>
#include <stdexcept>

namespace
{
void require(bool condition, const char* message)
{
    if (!condition) throw std::runtime_error(message);
}

void vmInspectionReportsDataAndTemporaryRegisters()
{
    std::array<byte, 24> program = {
        4, 0, 0, 0, 42, 0, 0, 0,
        1, 0, 5, 0, 0, 0, 0, 0,
        6, 0, 0, 0, 0, 0, 0, 0
    };
    VMState state;
    state.regs = Array<qword, dword>(8);
    state.programptr = program.data();
    state.len = program.size();

    MVM vm(state);
    require(vm.run(3), "VM rejected synchronous steps");
    const auto inspection = vm.inspect();
    require(inspection.state.regs[0] == program.size(), "inspection PC mismatch");
    require(inspection.currdata == 42, "inspection data register mismatch");
    require(inspection.temporaries.size() == 1, "inspection temporary count mismatch");
    require(inspection.temporaries[0].id == 0, "inspection temporary id mismatch");
    require(inspection.temporaries[0].value == 42, "inspection temporary value mismatch");
    require(inspection.temporaries[0].address != 0, "inspection temporary address is null");
}

void assemblyFailurePreservesLoadedSessionAndExecutionCanReset()
{
    myvm::lab::LabSession session;
    myvm::lab::AssemblyDiagnostic diagnostic;
    const std::string source = ".code\nldl #d42\nleareg r5, t0\nsave t0\npause\n";
    require(session.assemble(source, diagnostic), "valid program did not assemble");
    require(session.codeSize() == 32, "assembled program size mismatch");
    require(session.run(3, diagnostic) == 3, "VM did not execute three instructions");
    require(session.inspect().state.regs[5] == 42, "VM register result mismatch");
    require(session.trace().size() == 3, "execution history size mismatch");
    require(session.trace()[0].sequence == 1 && session.trace()[2].sequence == 3,
            "execution history sequence mismatch");

    const auto preservedSize = session.codeSize();
    require(!session.assemble(".code\nldl #missing\n", diagnostic), "invalid program assembled");
    require(diagnostic.line == 2, "assembly diagnostic line mismatch");
    require(session.codeSize() == preservedSize, "failed assembly replaced active program");
    require(session.inspect().state.regs[5] == 42, "failed assembly changed active VM state");

    require(session.run(10, diagnostic) == 1, "run did not stop after pause instruction");
    require(session.inspect().state.regs[0] == 32, "pause instruction PC mismatch");
    require(session.reset(diagnostic), "session reset failed");
    require(session.inspect().state.regs[0] == 0, "reset did not restore initial PC");
    require(session.inspect().state.regs[5] == 0, "reset did not clear general registers");
    require(session.trace().empty(), "reset did not clear execution history");
}

void disassemblyConsumesLeamemAddressLiteral()
{
    std::vector<byte> bytecode(24, 0);
    bytecode[0] = 2;
    bytecode[8] = 3;
    bytecode[16] = 4;
    bytecode[20] = 7;
    const auto rows = myvm::lab::decodeProgram(bytecode);
    require(rows.size() == 2, "leamem literal was decoded as an instruction");
    require(rows[0].offset == 0 && rows[0].size == 16, "leamem decoded size mismatch");
    require(rows[0].addressLiteral == 3, "leamem address literal mismatch");
    require(rows[1].offset == 16 && rows[1].mnemonic == "ldl", "instruction after leamem missing");
    require(rows[1].operands == "0x7", "ldl immediate decode mismatch");
}
}

int main()
{
    try
    {
        vmInspectionReportsDataAndTemporaryRegisters();
        assemblyFailurePreservesLoadedSessionAndExecutionCanReset();
        disassemblyConsumesLeamemAddressLiteral();
        std::cout << "MyVM lab model tests passed\n";
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return EXIT_FAILURE;
    }
}
