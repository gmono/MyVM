#include "mvm.h"
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <thread>

static int testArrayOwnership()
{
    Array<qword, dword> original(2);
    original[0] = 11;
    original[1] = 22;
    Array<qword, dword> copied(original);
    copied[0] = 99;
    if (original[0] != 11) return 1;

    Array<qword, dword> assigned;
    assigned = original;
    assigned[1] = 88;
    if (original[1] != 22) return 2;

    Array<qword, dword> moved;
    moved.movein(copied);
    if (!copied.isnull() || moved[0] != 99) return 3;
    return 0;
}

static int testInstructionExecution()
{
    byte program[] = {
        4, 0, 0, 0, 42, 0, 0, 0,       // ldl 42
        1, 0, 5, 0, 0, 0, 0, 0,        // leareg register 5, temp 0
        6, 0, 0, 0, 0, 0, 0, 0         // save temp 0
    };
    VMState state;
    state.regs = Array<qword, dword>(5 + 1);
    state.programptr = program;
    state.len = sizeof(program);

    MVM vm(state);
    if (!vm.run(3)) return 4;
    const VMState result = vm.getstate();
    if (result.regs[5] != 42 || result.regs[0] != sizeof(program)) return 5;
    return 0;
}

static int testTruncatedInstructionRejected()
{
    byte program[7] = {};
    VMState state;
    state.regs = Array<qword, dword>(5);
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    try {
        vm.run(1);
    } catch (const std::runtime_error&) {
        return 0;
    }
    return 6;
}

static int testMemoryAddressLiteral()
{
    byte program[40] = {};
    program[0] = 2; // leamem 0
    program[6] = 0;
    program[8] = 4; // qword index literal consumed after leamem
    program[16] = 4; // ldl 77
    program[20] = 77;
    program[24] = 6; // save 0 to memory[32]
    VMState state;
    state.regs = Array<qword, dword>(5);
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    if (!vm.run(4)) return 7;
    if (program[32] != 77) return 8;
    return 0;
}

static int testStackRoundTrip()
{
    byte program[64] = {};
    program[0] = 4; program[4] = 0x34; program[5] = 0x12; // ldl 0x1234
    program[8] = 1; program[10] = 5; // leareg register 5, temp 0
    program[16] = 16; // push
    program[24] = 4; // ldl 0
    program[32] = 17; // pop
    program[40] = 6; // save temp 0
    VMState state;
    state.regs = Array<qword, dword>(6);
    state.regs[3] = reinterpret_cast<qword>(program + sizeof(program));
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    if (!vm.run(6)) return 9;
    if (vm.getstate().regs[5] != 0x1234) return 10;
    return 0;
}

static int testAsyncStopAndWorkerError()
{
    byte loop[8] = {14, 0, 0, 0, 0, 0, 0, 0}; // jmp to currdata (zero)
    VMState state;
    state.regs = Array<qword, dword>(5);
    state.programptr = loop;
    state.len = sizeof(loop);
    MVM vm(state);
    vm.start();
    for (int i = 0; i < 100 && vm.havestop(); ++i)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    if (vm.havestop()) return 11;
    vm.stop();
    vm.wait();
    if (!vm.havestop()) return 12;
    vm.start();
    vm.stop();
    vm.wait();
    return 0;
}

static int testWorkerCapturesInvalidProgram()
{
    byte program[7] = {};
    VMState state;
    state.regs = Array<qword, dword>(5);
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    vm.start();
    vm.wait();
    if (!vm.havestop() || !vm.getworkererror()) return 13;
    try {
        std::rethrow_exception(vm.getworkererror());
    } catch (const VMError&) {
        return 0;
    }
    return 14;
}

static int testArithmeticAndConditionalBranch()
{
    byte program[160] = {};
    const auto instruction = [&](std::size_t offset, byte opcode, byte operand0 = 0,
                                 byte operand1 = 0, byte operand2 = 0, byte operand3 = 0)
    {
        program[offset] = opcode;
        if (opcode == 1)
        {
            program[offset + 2] = operand0;
            program[offset + 6] = operand1;
            return;
        }
        if (opcode == 15)
        {
            program[offset + 7] = operand0;
            return;
        }
        if (opcode == 2 || opcode == 3 || (opcode >= 6 && opcode <= 11))
        {
            program[offset + 6] = operand0;
            return;
        }
        program[offset + 4] = operand0;
        program[offset + 5] = operand1;
        program[offset + 6] = operand2;
        program[offset + 7] = operand3;
    };
    instruction(0, 4, 6);
    instruction(8, 1, 5, 0, 0, 0);
    instruction(16, 6);
    instruction(24, 4, 4);
    instruction(32, 8, 0);
    instruction(40, 4, 2);
    instruction(48, 10, 0);
    instruction(56, 4, 3);
    instruction(64, 9, 0);
    instruction(72, 4, 7);
    instruction(80, 7, 0);
    instruction(88, 4, 10);
    instruction(96, 11, 0);
    instruction(104, 4, 128);
    instruction(112, 15, 1);
    instruction(128, 4, 55);
    instruction(136, 1, 6, 1);
    instruction(144, 6, 1);

    VMState state;
    state.regs = Array<qword, dword>(7);
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    if (!vm.run(18)) return 15;
    const auto result = vm.getstate();
    if (result.regs[5] != 10) return 16;
    if (result.regs[6] != 55) return 25;
    if (result.regs[0] != 152) return 26;
    return 0;
}

static int testPortReadAndWrite()
{
    byte program[40] = {};
    program[0] = 18; program[4] = 9; // read port 9
    program[8] = 1; program[10] = 5; // leareg r5, t0
    program[16] = 6; // save t0
    program[24] = 4; program[28] = 0x34; // ldl 0x34
    program[32] = 19; program[36] = 9; // write port 9
    dword portValue = 0x12;
    dword writtenValue = 0;
    VMState state;
    state.regs = Array<qword, dword>(6);
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    IOPortInfo port{};
    port.port.portdata = &portValue;
    port.port.portset = [&](dword value) { writtenValue = value; };
    port.portid = 9;
    if (!vm.addport(port) || vm.addport(port)) return 17;
    if (!vm.run(3)) return 18;
    if (vm.getstate().regs[5] != 0x12) return 19;
    if (!vm.run(2)) return 20;
    if (writtenValue != 0x34) return 21;
    vm.setval(9, 0x56);
    if (writtenValue != 0x56) return 24;
    return 0;
}

static int testInterruptVectorAndReturn()
{
    byte program[64] = {};
    program[0] = 0; program[4] = 1; // rupt 1
    program[24] = 4; program[28] = 77; // handler: ldl 77
    program[32] = 1; program[34] = 6; program[38] = 1; // leareg r6, t1
    program[40] = 6; program[46] = 1; // save t1
    program[48] = 22; // iret
    qword vectors[2] = {0, 24};
    VMState state;
    state.regs = Array<qword, dword>(7);
    state.regs[1] = reinterpret_cast<qword>(vectors);
    state.regs[2] = 2;
    state.regs[3] = reinterpret_cast<qword>(program + sizeof(program));
    state.regs[4] = 4; // enable interrupt dispatch
    state.programptr = program;
    state.len = sizeof(program);
    MVM vm(state);
    if (!vm.run(5)) return 22;
    const auto result = vm.getstate();
    if (result.regs[6] != 77 || result.regs[0] != 8 || result.regs[3] != reinterpret_cast<qword>(program + sizeof(program))) return 23;
    return 0;
}

int main()
{
    try
    {
        std::cerr << "Array ownership\n";
        const int ownership = testArrayOwnership();
        if (ownership) return ownership;
        std::cerr << "instruction execution\n";
        const int instruction = testInstructionExecution();
        if (instruction) return instruction;
        std::cerr << "truncated instruction\n";
        const int truncated = testTruncatedInstructionRejected();
        if (truncated) return truncated;
        std::cerr << "memory addressing\n";
        const int memory = testMemoryAddressLiteral();
        if (memory) return memory;
        std::cerr << "stack\n";
        const int stack = testStackRoundTrip();
        if (stack) return stack;
        std::cerr << "async stop\n";
        const int async = testAsyncStopAndWorkerError();
        if (async) return async;
        std::cerr << "worker error\n";
        const int workerError = testWorkerCapturesInvalidProgram();
        if (workerError) return workerError;
        std::cerr << "arithmetic and branch\n";
        const int arithmetic = testArithmeticAndConditionalBranch();
        if (arithmetic) return arithmetic;
        std::cerr << "ports\n";
        const int ports = testPortReadAndWrite();
        if (ports) return ports;
        std::cerr << "interrupt\n";
        return testInterruptVectorAndReturn();
    }
    catch (const std::exception& error)
    {
        std::cerr << error.what() << '\n';
        return 99;
    }
}
