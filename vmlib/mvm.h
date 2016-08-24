#ifndef MVM_H
#define MVM_H
#define dllexport __declspec(dllexport)
#include <cstring>
#include <map>
#include <mutex>
#include <functional>
#define lockdo(fun) statelock.lock();fun;statelock.unlock()
//虚拟机核心的port允许32位数据传输，但是不同情况的高位可能无效
typedef unsigned int dword;
typedef unsigned long long qword;
typedef unsigned short word;
typedef unsigned char byte;
typedef std::function<void(dword)> IOPortSetFun;
//端口
struct IOPort
{
    dword *portdata;// 从这里读取数据
    IOPortSetFun portset;
};

struct IOPortInfo
{
    IOPort port;//端口
    dword portid;//请求的端口号，如果被占用就失败
};

typedef std::function<void()> PauseCallBack;//暂停指令回调函数
typedef std::function<void(qword currp)> PointCallBack;//断点指令回调函数,如果要处理断点 则在其中阻塞线程
///
/// \brief 寄存器状态结构 保存寄存器id和值
///
struct RegisterState
{
    dword id;
    qword value;
};
template<class T,class T2>
struct Array
{
    T *data;
    T2 sum;

	T* start() { return data; }
	T* end() { return data + sum; }
	//为了方便 本无必要
	void movein(Array<T, T2> &obj) { if (data) delete[] data; data = obj.data; sum = obj.sum; }
	bool isnull() { return data == nullptr; }
	Array(qword size) { 
		data = new T[size];
		sum = size; 
		for (int i = 0; i < size; ++i) 
			data[i] = 0;/*初始化为0 */
	}
    Array(){data=nullptr;sum=0;}
    //设定复制构造函数
    Array(Array &ar)
    {
        if(data==nullptr) delete data;
        data=new T[ar.sum];
        memcpy(data,ar.data,ar.sum);
        sum=ar.sum;
    }
	//索引器 从0开始 返回的是引用
	T &operator [](T2 index)
	{
		if (index >= sum) throw "错误，index越界!";//就这个处理了 不做别的 ,一般来说到这里就是程序出错了不然指令
		//函数会处理
		return data[index];
	}
	//取数据地址
	T *at(T2 index)
	{
		if (index >= sum) throw "错误，index越界!";//就这个处理了 不做别的
		return data+index;
	}
};
struct VMState
{
    VMState(){}
    VMState(VMState &s)
    {
        this->regs=s.regs;
        this->curpoint=s.curpoint;
        this->len=s.len;
        this->points=s.points;
        this->ports=s.ports;
        this->programptr=s.programptr;
		this->pointcbk = s.pointcbk;
		this->pausecbk = s.pausecbk;
    }
	//以下为两个回调函数
	//这两个函数必须被设置
	PointCallBack pointcbk;
    PauseCallBack pausecbk;
    //reg和port只能为dword数量 因为指令定常为qword 寄存器都是八字节大小
    Array<qword,dword> regs;

    //port因为不是连续的所以不能用array，要用map，为了稳定就用普通map了 二叉树搜索

    std::map<dword,IOPort> ports;

    //注意 断点表的每一个元素保存的是第几步要中断

    Array<qword,qword> points; //断点表
    qword curpoint=0;//当前准备中断断点表中的第几个
    //注意断点是按顺序排列的也就是 后面的项的值一定大于前面

    //注意下面的void指针可以被直接复制
    void *programptr=nullptr;//内存空间开始地址
    qword len=0;//内存空间大小

    qword nowstep=0;//当前步数记录器,注意这个标记是始终保持内外同步的，也就是每次进出静止状态都会复制一次
};

///
/// 特殊指令 pause step
/// 注意一切对虚拟机状态的获取和修改等都必须在静止状态下进行
/// 进入静止状态可以是pause指令执行或者 step指令生效
/// 以上规定是为了尽量避免读写volatile变量
/// 在进入静止状态时，执行线程会将状态装入同步变量副本内，以保持线程同步
///
///
/// 执行线程每次从静止返回后就会检查一次need标记
///
/// 原则很简单 即
/// 若此时执行线程没有运行 则以外部状态为准
/// 执行时以内部状态为准
/// run由于是短暂执行所以不复制直接 在外部状态里操作
/// 每次暂停或者进入断点或者因为调用stop标记而停止执行都是进入静止状态
/// 所以进入前都复制内部状态到外部
/// 而在进入静止状态后need系列变量标记的“需要复制到内部”都是false
/// 一旦某个项目被修改则对应标记改变，在从静止状态退出（重新开始执行或者从暂停 中断状态退出）时就会复制
/// 相应项目
/// run函数每次执行都会把所有标记设置为真
///
/// 外部中断会设置一个中断标记bool变量，执行函数每次在末尾都会检测这个变量，一旦为真则立即调用
/// 对应中断号存储变量的值对应的中断响应函数
///
/// stop会设置一个停止标记bool变量 执行函数末尾检查它为真则停止执行
/// 标记变量都是 volatile
///
/// 特殊寄存器如下 0 程序执行寄存器 保存当前执行位置指针
/// 1 中断向量表指针寄存器 2 中断向量表长度寄存器 3 栈指针寄存器 4 标志寄存器
/// 注意标志寄存器的最后两位是比较位
///
/// 注意必须在静止状态下修改状态才有效，否则进入静止状态将会用内部状态覆盖外部状态而内部状态可能与外部状态不同
///
///
/// 注意回调函数不在”状态“内
///
/// 注意此类只设计为一个线程操作
///
/// 一切修改状态的函数都只修改外部状态
///
/// 注意断点表必须是从小到大排序的
/// 否则将出现不可预料的错误
///
///
/// 注意 断点号从1开始 每一个指令集合后面有个step指令 使得状态中的nowstep标记加1  nowstep标记初始为0
class dllexport MVM
{

public:
    ///
    /// \brief MVM 初始化虚拟机核心
    MVM(VMState &state); //端口允许42亿个
    //端口相关
    bool addport(IOPortInfo info);
    void deleteport(IOPortInfo);
    bool exist(dword portid);//是否存在一个端口
    void setval(dword portid,dword value);//设置一个端口的值

    //属性相关
    ///
    /// \brief setprogram 设置内存空间
    /// \param pro 内存空间地址
    /// \param maxlen 内存空间大小
    ///
    void setprogram(void *pro,qword maxlen);
    void setpausecbk(PauseCallBack cbk);//设置回调函数，在特殊指令pause执行时被调用，最好立即返回

    //断点相关
    void setruptpoint(PointCallBack cbk);//设置回调函数，在断点生效时被调用
    void setpointtable(qword *list,qword sum);//设置断点描述表


    //中断相关
    ///
    /// \brief interrupt 外部中断函数,与其关联的有唯一一个循环读取的 volatile变量
    /// \return 返回是否成功，处于不响应中断状态时返回false
    ///
    ///
    bool interrupt(dword);

    //运行相关

    void start();//会建立一个新线程执行程序,立即返回,进入是检查need标记
    void stop();//这个会设置停止标记变量（volatile）为真，执行函数末尾会检查它
    bool havestop();//检测是否停止了 start函数里修改 run函数不修改
    bool run(dword steps);//单步执行函数,参数为执行几个单步,同步函数,只有在非start状态才能调用否则返回false
    //run函数进入时检查need标记 退出时复制所有状态


private:
    //volatile系列变量
    volatile bool isneed_ports=true;//是否需要从volatile变量复制端口信息到普通变量,下同
    volatile bool isneed_register=true;//寄存器
    volatile bool isneed_points=true;//断点
    volatile bool isneed_pro=true;//内存空间


    volatile bool isrupt=false;//是否开始中断,同时也是标记是否处于中断处理中的变量 内部中断不涉及此处的两个变量
    //以上变量在外部中断函数里被设置为真，在外部中断处理完后被设置为假
    volatile dword ruptid=0;//中断号
	//所谓的外部中断也包括除0 之内的vm程序性中断
	volatile bool isinrupt = false;//标记是否在中断中

	void startrupt(dword id);//使用指定id开始一个外部中断

    volatile bool isstop=false;//要停止时设置为真 停止后设置为假 如果一开始就为真则执行线程不会做任何事情
    volatile bool havestoped=true;//标记是否已经


    //采用另一种方法 保证线程不会同时操作这两个表
    std::mutex statelock;//操作锁
    /*volatile*/ VMState outstate;
    VMState instate;
    //以上为内外状态


    //复制相关函数

    //只有复制入函数，复制出来只需要等于即可

    void copyin();


    ///注意此处已经为执行函数部分开路
    /// 保持清晰状态





//这里有几点要注意 1 寄存器号是dword 2 内存地址是 qword


    void doone();//执行一个指令

    //以下为暂存器
    qword *ptrs[65536];//允许16位标记的内存暂存器地址（可以进行simd操作）
    bool getmptr=false;//取内存地址标记暂存器
    word getmpi=0;//取内存地址目标索引暂存器 存储从内存地址得到的通用指针存放的内存地址暂存器的id

    qword currdata=0;//数据暂存器
    //以下为指令执行函数系列
    void rupt(dword id);//进入中断,保存当前执行位置到栈中并从中断向量表中取得中断并跳转到对应位置执行
    void leareg(dword regid,word id);//得到一个寄存器地址放到对应的地址暂存器（指针）
    void leamem(word id);//设置取内存地址标记暂存器，并把id放入索引暂存器，下一个指令为内存地址（8字节）


    void ld(word id);//取数据指令,从id指明的地址暂存器中得到地址并且用这个地址取得数据(8字节）放入数据暂存器
    void ldl(dword data);//直接设置数据暂存器的低位
    void ldh(dword data);//直接设置数据暂存器的高位
    void save(word id);//存数据指令，将数据暂存器中的数据存入 id指明的地址暂存器对应的位置

    //以下为计算函数

    void add(word id);//累加 将数据暂存器中的数据累加到对应位置
    void dec(word id);//减 同上
    void mul(word id);//乘
    void div(word id);//除法
//以下数据代表数据寄存器的数据
    //标志相关
    byte zero=0;//数据相等
    byte low=0;//数据较小
    //如果以上都为false则代表数据较大
    void cmp(word id);//用数据暂存器的数据减去对应位置数据 并设置标志暂存器 结果大于0则
    //以下为特殊指令
    void pause(); //暂停指令
    void step(); //增步指令
    //以下为操作指令
    void jmp();//直接跳转到数据指明的位置
    void asjmp(byte d);//如果比较标志寄存器的比较位和d的最后两位相等则跳转
    //以下为栈操作指令
    void push();//将数据放入栈
    void pop();//将栈指针指向的数据放入数据数据暂存器
    //以下为simd指令,暂时还没写

	//以下为接口操作
	void read(dword id);//端口读 读取到数据暂存器
	void write(dword id);//端口写 将数据暂存器的数据写入到端口

	void call();//函数跳转 
	void ret();//函数返回
	void iret();//中断返回

	//辅助函数系列
	void tojmp(qword ptr);//通用跳转函数
	void tocall(qword ptr);//通用有返回跳转

	//取地址特殊函数
	void getmemptr(qword ptr);//取地址的函数  //在取地址标记为真时被调用(只要为真 任何指令都被当成地址），将地址原码对应的内存地址指针放入目标索引对应的地址暂存器
};

#endif // MVM_H
