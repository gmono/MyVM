#include "mvm.h"

#include <cstdint>
#include <cstring>
#include <iostream>

namespace
{
word readWord(const byte* bytes)
{
    return static_cast<word>(bytes[0]) |
        (static_cast<word>(bytes[1]) << 8);
}

dword readDword(const byte* bytes)
{
    return static_cast<dword>(bytes[0]) |
        (static_cast<dword>(bytes[1]) << 8) |
        (static_cast<dword>(bytes[2]) << 16) |
        (static_cast<dword>(bytes[3]) << 24);
}

qword readQword(const byte* bytes)
{
    qword value = 0;
    for (unsigned i = 0; i < 8; ++i) value |= static_cast<qword>(bytes[i]) << (i * 8);
    return value;
}

}

bool MVM::validProgramRange(qword offset, qword size) const
{
    return instate.programptr != nullptr && offset <= instate.len && size <= instate.len - offset;
}

byte* MVM::gettemp(word id)
{
    byte* result = ptrs[id];
    if (result == nullptr) throw VMError("temporary register is not initialized");
    return result;
}

void MVM::doone()
{
    bool hasInterrupt = false;
    dword interruptId = 0;
    {
        std::lock_guard<std::mutex> guard(interruptlock);
        if (isrupt)
        {
            hasInterrupt = true;
            interruptId = ruptid;
            isrupt = false;
        }
    }
    if (hasInterrupt)
    {
        if (!isinrupt) { rupt(interruptId); return; }
    }

    if (!instate.points.isnull() && instate.curpoint < instate.points.sum &&
        instate.nowstep == instate.points[instate.curpoint])
    {
        {
            std::lock_guard<std::mutex> guard(statelock);
            outstate = instate;
        }
        if (instate.pointcbk) instate.pointcbk(instate.curpoint);
        ++instate.curpoint;
    }

    if (instate.regs.sum < 5) throw VMError("VM state has fewer than five registers");
    qword& pc = instate.regs[0];
    if (!validProgramRange(pc, 8)) throw VMError("instruction fetch is outside program memory");
    const auto* instruction = static_cast<const byte*>(instate.programptr) + pc;

    // leamem makes the following eight-byte word an address literal, not an opcode.
    if (getmptr)
    {
        const qword address = readQword(instruction);
        getmemptr(address);
        getmptr = false;
        pc += 8;
        return;
    }

    const byte opcode = instruction[0];
    pc += 8;
    switch (opcode)
    {
    case 0:
        if (!isinrupt) rupt(readDword(instruction + 4));
        break;
    case 1: leareg(readDword(instruction + 2), readWord(instruction + 6)); break;
    case 2: leamem(readWord(instruction + 6)); break;
    case 3: ld(readWord(instruction + 6)); break;
    case 4: ldl(readDword(instruction + 4)); break;
    case 5: ldh(readDword(instruction + 4)); break;
    case 6: save(readWord(instruction + 6)); break;
    case 7: add(readWord(instruction + 6)); break;
    case 8: dec(readWord(instruction + 6)); break;
    case 9: mul(readWord(instruction + 6)); break;
    case 10: div(readWord(instruction + 6)); break;
    case 11: cmp(readWord(instruction + 6)); break;
    case 12: pause(); break;
    case 13: step(); break;
    case 14: jmp(); break;
    case 15: asjmp(instruction[7]); break;
    case 16: push(); break;
    case 17: pop(); break;
    case 18: read(readDword(instruction + 4)); break;
    case 19: write(readDword(instruction + 4)); break;
    case 20: call(); break;
    case 21: ret(); break;
    case 22: iret(); break;
    case 23: bitAnd(readWord(instruction + 6)); break;
    case 24: bitOr(readWord(instruction + 6)); break;
    case 25: bitXor(readWord(instruction + 6)); break;
    case 26: shl(readWord(instruction + 6)); break;
    case 27: shr(readWord(instruction + 6)); break;
    case 28: bitnot(readWord(instruction + 6)); break;
    default: startrupt(0); break;
    }
}

void MVM::rupt(dword id)
{
    if (instate.regs.sum < 5) throw VMError("VM state has fewer than five registers");
    if ((instate.regs[4] & (static_cast<qword>(1) << 2)) == 0 && id <= 4)
    {
        static const char* const errors[] = {
            "invalid instruction", "division by zero", "port not found",
            "invalid register or memory reference", "interrupt vector out of range"
        };
        std::cerr << errors[id] << '\n';
        return;
    }

    const qword vectorCount = instate.regs[2];
    const qword vectorAddress = instate.regs[1];
    if (id >= vectorCount || vectorAddress == 0)
        throw VMError("interrupt vector is outside the configured table");
    if (id > (std::numeric_limits<std::uintptr_t>::max() - vectorAddress) / sizeof(qword))
        throw VMError("interrupt vector address overflow");

    qword target = 0;
    std::memcpy(&target, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(vectorAddress) + id * sizeof(qword)), sizeof(target));
    isinrupt = true;
    tocall(target);
}

void MVM::leareg(dword registerId, word temporaryId)
{
    if (registerId >= instate.regs.sum)
        throw VMError("register index " + std::to_string(registerId) + " is outside register file of size " +
                      std::to_string(instate.regs.sum));
    ptrs[temporaryId] = reinterpret_cast<byte*>(instate.regs.at(registerId));
}

void MVM::leamem(word id) { getmpi = id; getmptr = true; }

void MVM::ld(word id) { std::memcpy(&currdata, gettemp(id), sizeof(currdata)); }

void MVM::ldl(dword data)
{
    currdata = (currdata & 0xffffffff00000000ULL) | data;
}

void MVM::ldh(dword data)
{
    currdata = (currdata & 0x00000000ffffffffULL) | (static_cast<qword>(data) << 32);
}

void MVM::save(word id) { std::memcpy(gettemp(id), &currdata, sizeof(currdata)); }
void MVM::add(word id)
{
    qword value;
    std::memcpy(&value, gettemp(id), sizeof(value));
    value += currdata;
    std::memcpy(gettemp(id), &value, sizeof(value));
}

void MVM::dec(word id)
{
    qword value;
    std::memcpy(&value, gettemp(id), sizeof(value));
    value -= currdata;
    std::memcpy(gettemp(id), &value, sizeof(value));
}

void MVM::mul(word id)
{
    qword value;
    std::memcpy(&value, gettemp(id), sizeof(value));
    value *= currdata;
    std::memcpy(gettemp(id), &value, sizeof(value));
}

void MVM::div(word id)
{
    if (currdata == 0) { startrupt(1); return; }
    qword value;
    std::memcpy(&value, gettemp(id), sizeof(value));
    value /= currdata;
    std::memcpy(gettemp(id), &value, sizeof(value));
}

void MVM::cmp(word id)
{
    qword value;
    std::memcpy(&value, gettemp(id), sizeof(value));
    qword& flags = instate.regs[4];
    flags &= ~static_cast<qword>(3);
    if (currdata > value) flags |= 2;
    else if (currdata == value) flags |= 1;
}

void MVM::pause()
{
    {
        std::lock_guard<std::mutex> guard(statelock);
        outstate = instate;
    }
    if (instate.pausecbk) instate.pausecbk();
}

void MVM::step() { ++instate.nowstep; }
void MVM::jmp() { tojmp(currdata); }

void MVM::asjmp(byte condition)
{
    if ((condition & 3) == (instate.regs[4] & 3)) jmp();
}

void MVM::push()
{
    if (!instate.programptr) throw VMError("stack memory is not configured");
    const auto base = reinterpret_cast<std::uintptr_t>(instate.programptr);
    const auto pointer = static_cast<std::uintptr_t>(instate.regs[3]);
    if (instate.len > std::numeric_limits<std::uintptr_t>::max() - base ||
        pointer < base || pointer - base < sizeof(qword) || pointer - base > instate.len)
        throw VMError("stack pointer is outside program memory");
    const auto next = pointer - sizeof(qword);
    std::memcpy(reinterpret_cast<void*>(next), &currdata, sizeof(currdata));
    instate.regs[3] = next;
}

void MVM::pop()
{
    if (!instate.programptr) throw VMError("stack memory is not configured");
    const auto base = reinterpret_cast<std::uintptr_t>(instate.programptr);
    const auto pointer = static_cast<std::uintptr_t>(instate.regs[3]);
    if (instate.len > std::numeric_limits<std::uintptr_t>::max() - base ||
        pointer < base || pointer - base > instate.len || instate.len - (pointer - base) < sizeof(qword))
        throw VMError("stack pointer is outside program memory");
    std::memcpy(&currdata, reinterpret_cast<const void*>(pointer), sizeof(currdata));
    instate.regs[3] = pointer + sizeof(qword);
}

void MVM::read(dword id)
{
    const auto port = instate.ports.find(id);
    if (port == instate.ports.end() || port->second.portdata == nullptr) { startrupt(2); return; }
    currdata = *port->second.portdata;
}

void MVM::write(dword id)
{
    const auto port = instate.ports.find(id);
    if (port == instate.ports.end()) { startrupt(2); return; }
    if (port->second.portset) port->second.portset(static_cast<dword>(currdata));
}

void MVM::call() { tocall(currdata); }

void MVM::ret()
{
    const qword savedData = currdata;
    pop();
    tojmp(currdata);
    currdata = savedData;
}

void MVM::iret() { isinrupt = false; ret(); }
void MVM::bitAnd(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value &= currdata; std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::bitOr(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value |= currdata; std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::bitXor(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value ^= currdata; std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::shl(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value <<= (currdata & 63); std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::shr(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value >>= (currdata & 63); std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::bitnot(word id) { qword value; std::memcpy(&value, gettemp(id), sizeof(value)); value = ~value; std::memcpy(gettemp(id), &value, sizeof(value)); }
void MVM::tojmp(qword pointer) { instate.regs[0] = pointer; }

void MVM::tocall(qword pointer)
{
    const qword savedData = currdata;
    currdata = instate.regs[0];
    push();
    currdata = savedData;
    tojmp(pointer);
}

void MVM::getmemptr(qword pointer)
{
    if (pointer > instate.len / sizeof(qword)) throw VMError("memory operand is outside program memory");
    const auto offset = pointer * sizeof(qword);
    if (!validProgramRange(offset, sizeof(qword))) throw VMError("memory operand is outside program memory");
    ptrs[getmpi] = static_cast<byte*>(instate.programptr) + offset;
}
