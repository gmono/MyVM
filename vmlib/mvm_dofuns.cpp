#include "mvm.h"
///注意以下为执行函数系列！！
#include <iostream>
#pragma region 指令执行函数
void MVM::doone()
{
	//注意 这个函数只操作内部状态
	//开始前分别检查isrupt标记 和 nowstep标记（在内部状态中）
	//遵循先中断后停步的原则
	if (isrupt)
	{
		if (!isinrupt)
		{
			//如果不在中断中就开始中断
			rupt(ruptid);
			isrupt = false;
			return;//开始中断执行 这个指令不执行直接返回
		}
		else isrupt = false;//不管是否响应了中断都直接设置此标志 如果正在中断则代表忽略中断请求
		////以后可能会加上关于中断请求模式（直接和队列式）的功能
	}
	if (!instate.points.isnull()&&instate.nowstep == instate.points[instate.curpoint])
	{
		//调用断点回调函数
		lockdo(outstate = instate);
		instate.pointcbk(instate.curpoint); //这里不考虑没有设置断点回调函数的情况 也就是说必须设置
		//如果程序处理中断则可以在其中阻塞线程
	}
	//以下处理指令
	//获取指令
	//获取执行指针寄存器的引用
	qword &pptr = instate.regs[0];
	if (pptr >= instate.len) throw "错误，程序执行越界！";//如果执行超过定义的内存空间大小则抛出异常
	byte *coms =((byte *)instate.programptr)+pptr;//得到当前指令的指针

	if (getmptr) getmemptr(*(qword *)coms);//如果有取内存地址标记就直接取地址

#define getid(type,pt) *(type *)(coms+pt)
 	//获取指令码 也就是第一个字节
	byte com = coms[0];
	switch (com)
	{
	case 0:
		//内部中断指令
		//先表示此指令已经处理完了
		pptr+=8;
		if (isinrupt) break;//如果正在中断就直接不管它
		//否则启动中断
		//获取中断码
		{
			//由于case不能定义变量故用了一个语句块
			dword id = getid(dword,4);//得到中断id
			rupt(id);//进入中断过程
		};
		break;
	case 1:
		pptr+=8;
		{
			dword regid = getid(dword,2);//获得寄存器id
			word sid = getid(word,6);//获得暂存器id
			leareg(regid, sid);
		};
		break;
	case 2:
		pptr+=8;
		{
			word id = getid(word, 6);
			leamem(id);
		};
		break;
	case 3:
		pptr+=8;
		{
			word id = getid(word, 6);
			ld(id);
		};
		break;
	case 4:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			ldl(id);
		};
		break;
	case 5:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			ldh(id);
		};
		break;
	case 6:
		pptr+=8;
		{
			word id = getid(word, 6);
			save(id);
		};
		break;
	case 7:
	case 8:
	case 9:
	case 10:
	case 11:
		pptr+=8;
		{
			word id = getid(word, 6);
			decltype(&MVM::add) fun;
			switch (com)
			{
			case 7:
				fun = &MVM::add;
				break;
			case 8:
				fun = &MVM::dec;
				break;
			case 9:
				fun = &MVM::mul;
				break;
			case 10:
				fun =&MVM::div;
				break;
			case 11:
				fun = &MVM::cmp;
				break;
			}
			//这里不考虑其他情况
			(this->*fun)(id);//直接调用
		};
		break;
	case 12:
		pptr+=8;
		pause();
		break;
	case 13:
		pptr+=8;
		step();
		break;
	case 14:
		pptr+=8;
		jmp();
		break;
	case 15:
		pptr+=8;
		{
			byte id = getid(byte, 7);
			asjmp(id);
		}
		break;
	case 16:
		pptr+=8;
		push();
		break;
	case 17:
		pptr+=8;
		pop();
		break;
	case 18:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			read(id);
		}
		break;
	case 19:
		pptr+=8;
		{
			dword id = getid(dword, 4);
			write(id);
		}
		break;
	case 20:
		pptr+=8;
		call();
		break;
	case 21:
		pptr+=8;
		ret();
		break;
	case 22:
		pptr+=8;
		iret();
		break;
	default:
		pptr+=8;
		//指令错误 触发指令错误中断 外部中断0号
		startrupt(0);
		break;
	}
	//在此不做任何事情 
	//标准设定为指令执行完不发生任何额外事情
	//任何额外事件都在下一个指令开始前执行
}
#pragma endregion

//特殊寄存器如下 0 程序执行寄存器 保存当前执行位置指针(相对程序起始地址)
// 1 中断向量表指针寄存器  2 中断向量表长度寄存器 3 栈指针寄存器 4 标志寄存器 
//标志寄存器最后两位为比较位 倒数第三位是特殊中断响应位 为真时特殊中断也调用中断向量表

//内部中断指令处理函数
void MVM::rupt(dword id)
{
	auto a = (instate.regs[4] & (((qword)1) << 2));
	if ((instate.regs[4] & (((qword)1) << 2)) == 0)
	{
		//处理特殊中断指令
		switch (id)
		{
		case 0:
			std::cerr << "指令错误！";
			return;
		case 1:
			std::cerr << "除0错误！";
			return;
		case 2:
			std::cerr << "端口不存在";
			return;
		case 3:
			std::cerr << "寄存器不存在";
			return;
		case 4:
			std::cerr << "中断号超界";
			return;
		}
	}
	//从中断向量表得到地址
	qword tabp = instate.regs[1];
	qword len = instate.regs[2];
	if (id >= len) 
	{
		startrupt(4); return;
	}//向量号超界
	//否则开始中断
	qword data = ((qword *)tabp)[id];//得到中断函数地址
	isinrupt = true;//设置标志
	//跳转
	tocall(data);
	
}

void MVM::leareg(dword regid, word id)
{
	if (regid >= instate.regs.sum) { startrupt(3); return; }
	qword *ptr = instate.regs.at(regid);
	ptrs[id] = ptr;
	//暂存器越界不存在 因为有65536个
}

void MVM::leamem(word id)
{
	getmpi = id;
	getmptr = true;
}


void MVM::ld(word id)
{
	currdata = *ptrs[id];
}

void MVM::ldl(dword data)
{
	*(dword*)(&currdata) = data;
}

void MVM::ldh(dword data)
{
	*(dword*)(&currdata+4) = data;
}

void MVM::save(word id)
{
	*ptrs[id] = currdata;
}

void MVM::add(word id)
{
	*ptrs[id] += currdata;
}

void MVM::dec(word id)
{
	*ptrs[id] -= currdata;
}

void MVM::mul(word id)
{
	*ptrs[id] *= currdata;
}

void MVM::div(word id)
{
	*ptrs[id] /= currdata;
}
//index从0开始
#define settrue(obj,index) obj=obj|((qword)1<<index)
#define setfalse(obj,index) obj=obj&(~((qword)1<<index))
void MVM::cmp(word id)
{
	//这里进行比较
	if (currdata > *ptrs[id])
	{
		settrue(instate.regs[4], 1);
		setfalse(instate.regs[4], 0);
		//10代表大于
	}
	else if (currdata == *ptrs[id])
	{
		settrue(instate.regs[4], 0);
		setfalse(instate.regs[4], 1);
	}
	else
	{
		setfalse(instate.regs[4], 0);
		setfalse(instate.regs[4], 1);
	}
	//两个都为真的情况不存在
}

void MVM::pause()
{
	//复制出
	lockdo(outstate = instate);
	instate.pausecbk();//调用暂停函数
}

void MVM::step()
{
	instate.nowstep++;
}

void MVM::jmp()
{
	tojmp(currdata);
}

void MVM::asjmp(byte d)
{
	//比较最后两位 由于只用一次所以没有写成宏
	if ((d & 0b00000011) == ((byte)instate.regs[4]) & 0b00000011)
	{
		jmp();
	}
}

void MVM::push()
{
	qword *stackptr_ptr = instate.regs.at(3);
	qword *&stackptr = *(qword **)stackptr_ptr;
	stackptr--;//从高到低 先改变指针再入栈
	*stackptr = currdata;

}

void MVM::pop()
{
	qword *stackptr_ptr = instate.regs.at(3);
	qword *&stackptr = *(qword **)stackptr_ptr;
	currdata=*stackptr;
	stackptr++;//先取栈顶再加栈指针 从低到高
}

void MVM::read(dword id)
{
	//获得端口的数据地址
	auto ptr = instate.ports.find(id);
	if (ptr == instate.ports.end()) { startrupt(2); return; }
	IOPort port = ptr->second;
	dword data = *port.portdata;
	//放入数据暂存器
	currdata = data;//放到暂存器的低位部分
}

void MVM::write(dword id)
{
	//复制粘贴的代码 其实可以把上面的一部分代码封装成一个函数
	//获得端口设置函数
	auto ptr = instate.ports.find(id);
	if (ptr == instate.ports.end()) { startrupt(2); return; }
	IOPort port = ptr->second;
	auto fun = port.portset;

	//设置数据 注意此处假设“硬件”接口响应函数不会读取和修改外部状态 也就是不参与虚拟机的监视活动
	fun(currdata);
}

void MVM::call()
{
	tocall(currdata);
}

void MVM::ret()
{
	auto temp = currdata;
	pop();//得到返回地址
	tojmp(currdata);//跳转设置
	currdata = temp;//还原数据
}

void MVM::iret()
{
	isinrupt = false;
	ret();
}

void MVM::tojmp(qword ptr)
{
	instate.regs[0] = ptr;//跳转
}

void MVM::tocall(qword ptr)
{
	qword reg0 = instate.regs[0];
	auto temp = currdata;
	currdata = reg0;
	push();
	currdata = temp;
	tojmp(ptr);//先保存后跳转
}

void MVM::getmemptr(qword ptr)
{
	qword *p = (qword*)((byte *)instate.programptr) + ptr;
	ptrs[getmpi]=p;
}
