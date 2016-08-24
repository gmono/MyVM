#include "mvm.h"
#include <thread>


//tmd结构体没有初始构造
MVM::MVM(VMState &state):outstate(state)
{
	if (state.regs.sum < 3) throw "reg sum is too few";
}

bool MVM::addport(IOPortInfo info)
{
    statelock.lock();
    if(outstate.ports.find(info.portid)!=outstate.ports.end()) return false;//如果被占用就返回false;
    outstate.ports.insert(std::pair<dword,IOPort>(info.portid,info.port));//放进去
    //注意这步最重要

    statelock.unlock();
    isneed_ports=true;//设置port的need标记
    return true;
}

void MVM::deleteport(IOPortInfo info)
{
    statelock.lock();
    if(outstate.ports.find(info.portid)==outstate.ports.end()) return;//没有就返回;
    outstate.ports.erase(info.portid);//删除
    //注意这步最重要

    statelock.unlock();
    isneed_ports=true;//设置port的need标记
}

bool MVM::exist(dword portid)
{
    bool ret;
    lockdo(ret=outstate.ports.find(portid)!=outstate.ports.end());
    return ret;
}

void MVM::setval(dword portid, dword value)
{
    if(exist(portid))
    {

        lockdo(IOPort &p=outstate.ports[portid];p.portset(value));//调用设置函数
        isneed_ports=true;//设置标记
    }
}

void MVM::setprogram(void *pro, qword maxlen)
{
    lockdo(outstate.programptr=pro;outstate.len=maxlen);
    isneed_pro=true;//设置标记
}

void MVM::setpausecbk(PauseCallBack cbk)
{
	outstate.pausecbk = cbk;
}

void MVM::setruptpoint(PointCallBack cbk)
{
	outstate.pointcbk = cbk;
}

void MVM::setpointtable(qword *list, qword sum)
{
    Array<qword,qword> ar;
    ar.data=list;
    ar.sum=sum;

    lockdo(outstate.points=ar);

    //设置标记
    isneed_points=true;
}

bool MVM::interrupt(dword id)
{
    if(isrupt) return false;
    ruptid=id;//设置中断号
    isrupt=true;//设置进入中断
    return true;
}

void MVM::start()
{
    std::thread th([this](){
        havestoped=false;
        //线程一开始就复制入
        lockdo(copyin());

        while(!isstop)
        {
            //无限循环执行 直到stop标记为真
            this->doone();
        }
        //结束时复制出
        lockdo(outstate=instate);
        isstop=false;
        havestoped=true;
    });
    th.detach();//分离，自生自灭

}

void MVM::stop()
{
    isstop=true;
}

bool MVM::havestop()
{
    return havestoped;
}

bool MVM::run(dword steps)
{
    //复制进入
    copyin();
    for(dword i=0;i<steps;++i)
    {
        this->doone();
    }
    outstate=instate;//由于不是多线程执行 不需要锁定
    return true;
}


void MVM::startrupt(dword id)
{
	ruptid = id;
	isrupt = true;
}

void MVM::copyin()
{
	if (isneed_register) {
		instate.regs = outstate.regs; isneed_register = false;
	}
	if (isneed_points) { instate.points = outstate.points; instate.curpoint = outstate.curpoint; isneed_points = false; }
	if (isneed_ports) {
		instate.ports = outstate.ports; isneed_ports = false;
	}
	if (isneed_pro) { instate.programptr = outstate.programptr; instate.len = outstate.len; isneed_pro = false; }
	//以下内容只在复制入时有意义 出时无意义 因此可以每次都向内复制进入
    instate.nowstep=outstate.nowstep;
	instate.pointcbk = outstate.pointcbk;
	instate.pausecbk = outstate.pausecbk;
	
}

