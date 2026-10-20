#include "mvm.h"

#include <algorithm>

MVM::MVM(const VMState& state) : outstate(state)
{
    if (state.regs.sum < 5) throw std::invalid_argument("VM state requires at least five registers");
    if (!outstate.pausecbk) outstate.pausecbk = [] {};
    if (!outstate.pointcbk) outstate.pointcbk = [](qword) {};
    std::fill(std::begin(ptrs), std::end(ptrs), nullptr);
}

MVM::~MVM()
{
    stop();
    wait();
}

VMState MVM::getstate() const
{
    std::lock_guard<std::mutex> guard(statelock);
    return outstate;
}

MVMInspection MVM::inspect() const
{
    std::unique_lock<std::mutex> executionGuard(executionlock, std::try_to_lock);
    if (!executionGuard.owns_lock())
        throw std::logic_error("VM inspection requires a stopped execution state");

    std::lock_guard<std::mutex> stateGuard(statelock);
    MVMInspection inspection;
    inspection.state = outstate;
    inspection.currdata = currdata;
    for (std::size_t id = 0; id < std::size(ptrs); ++id)
    {
        if (!ptrs[id]) continue;
        qword value = 0;
        std::memcpy(&value, ptrs[id], sizeof(value));
        inspection.temporaries.push_back({static_cast<word>(id),
                                          reinterpret_cast<qword>(ptrs[id]), value});
    }
    return inspection;
}

std::exception_ptr MVM::getworkererror() const
{
    std::lock_guard<std::mutex> guard(statelock);
    return workererror;
}

bool MVM::addport(IOPortInfo info)
{
    std::lock_guard<std::mutex> guard(statelock);
    if (outstate.ports.find(info.portid) != outstate.ports.end()) return false;
    outstate.ports.insert(std::make_pair(info.portid, info.port));
    isneed_ports = true;
    return true;
}

void MVM::deleteport(IOPortInfo info)
{
    std::lock_guard<std::mutex> guard(statelock);
    if (outstate.ports.erase(info.portid) != 0) isneed_ports = true;
}

bool MVM::exist(dword portid)
{
    std::lock_guard<std::mutex> guard(statelock);
    return outstate.ports.find(portid) != outstate.ports.end();
}

void MVM::setval(dword portid, dword value)
{
    IOPortSetFun callback;
    {
        std::lock_guard<std::mutex> guard(statelock);
        const auto port = outstate.ports.find(portid);
        if (port == outstate.ports.end()) return;
        callback = port->second.portset;
        isneed_ports = true;
    }
    if (callback) callback(value);
}

void MVM::setprogram(void* program, qword maxlen)
{
    if (maxlen != 0 && program == nullptr) throw std::invalid_argument("program pointer is null");
    std::lock_guard<std::mutex> guard(statelock);
    outstate.programptr = program;
    outstate.len = maxlen;
    isneed_pro = true;
}

void MVM::setpausecbk(PauseCallBack callback)
{
    std::lock_guard<std::mutex> guard(statelock);
    outstate.pausecbk = callback ? callback : PauseCallBack([] {});
}

void MVM::setruptpoint(PointCallBack callback)
{
    std::lock_guard<std::mutex> guard(statelock);
    outstate.pointcbk = callback ? callback : PointCallBack([](qword) {});
}

void MVM::setpointtable(qword* list, qword sum)
{
    if (sum != 0 && list == nullptr) throw std::invalid_argument("breakpoint list is null");
    Array<qword, qword> points(sum);
    if (sum) std::copy(list, list + sum, points.data);
    std::lock_guard<std::mutex> guard(statelock);
    outstate.points = std::move(points);
    outstate.curpoint = 0;
    isneed_points = true;
}

bool MVM::interrupt(dword id)
{
    std::lock_guard<std::mutex> guard(interruptlock);
    if (isrupt) return false;
    ruptid = id;
    isrupt = true;
    return true;
}

void MVM::start()
{
    std::lock_guard<std::mutex> lifecycleGuard(lifecyclelock);
    if (worker.joinable())
    {
        if (!havestoped.load()) return;
        worker.join();
    }
    {
        std::lock_guard<std::mutex> stateGuard(statelock);
        workererror = nullptr;
        isstop.store(false);
        havestoped.store(false);
    }
    try
    {
        worker = std::thread([this]()
        {
            std::unique_lock<std::mutex> executionGuard(executionlock);
            try
            {
                {
                    std::lock_guard<std::mutex> stateGuard(statelock);
                    copyin();
                }
                while (!isstop.load()) doone();
            }
            catch (...)
            {
                std::lock_guard<std::mutex> stateGuard(statelock);
                workererror = std::current_exception();
            }
            {
                std::lock_guard<std::mutex> stateGuard(statelock);
                outstate = instate;
            }
            isstop.store(true);
            havestoped.store(true);
        });
    }
    catch (...)
    {
        havestoped.store(true);
        isstop.store(true);
        throw;
    }
}

void MVM::stop()
{
    isstop.store(true);
}

void MVM::wait()
{
    std::lock_guard<std::mutex> guard(lifecyclelock);
    if (worker.joinable())
    {
        if (worker.get_id() == std::this_thread::get_id())
            throw std::logic_error("VM worker cannot wait for itself");
        worker.join();
    }
}

bool MVM::havestop()
{
    return havestoped.load();
}

bool MVM::run(dword steps)
{
    std::unique_lock<std::mutex> executionGuard(executionlock, std::try_to_lock);
    if (!executionGuard.owns_lock() || !havestoped.load()) return false;
    {
        std::lock_guard<std::mutex> stateGuard(statelock);
        copyin();
    }
    try
    {
        for (dword i = 0; i < steps; ++i) doone();
    }
    catch (...)
    {
        std::lock_guard<std::mutex> stateGuard(statelock);
        outstate = instate;
        throw;
    }
    {
        std::lock_guard<std::mutex> stateGuard(statelock);
        outstate = instate;
    }
    return true;
}

void MVM::startrupt(dword id)
{
    std::lock_guard<std::mutex> guard(interruptlock);
    ruptid = id;
    isrupt = true;
}

void MVM::copyin()
{
    if (isneed_register) { instate.regs = outstate.regs; isneed_register = false; }
    if (isneed_points) { instate.points = outstate.points; instate.curpoint = outstate.curpoint; isneed_points = false; }
    if (isneed_ports) { instate.ports = outstate.ports; isneed_ports = false; }
    if (isneed_pro) { instate.programptr = outstate.programptr; instate.len = outstate.len; isneed_pro = false; }
    instate.nowstep = outstate.nowstep;
    instate.pointcbk = outstate.pointcbk;
    instate.pausecbk = outstate.pausecbk;
}
