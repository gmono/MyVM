# MyVM
一个专门设计用来研究和实验的VM底层库 以及基础汇编器 还有测试程序还有莫名其妙的错误。奇怪。。指令解析应该没错啊 
# 目的说明
其实这个VM是很好调试的，因为其设计之初就不是为了性能，而是为了调试和监视的方便而设计的，本来想上面实现各种CPU体系的实验VM，因此设计的比较复杂，特别是在中断方面添加了非常复杂的逻辑，同时将内部所有状态对外暴露，同时还提供异步运行，还提供随时随地的中断和监视能力，因此内部逻辑复杂度可能比较高，目前有些BUG尚未修复，有感兴趣的，或者对汇编感兴趣的可以试试修复，汇编器没有错误，由于以前对编译原理理解不足，个人对字符串处理的能力所限，直接使用的C#的string操作函数，进行的大块式的操作，看起来可能不是那么优雅，有感兴趣的可以自己写个汇编器

# 重启计划
MyVM此前由于知识所限和思想局限，代码设计方面并没有很优雅，但其设计架构可取，故于19年12月4日考虑对其重启，最远在一年内重启
## 修正方面
将会在以下方面进行修正：
1. 指令函数不再接收多参数，而是接受一个与其配套的位结构体做参数
2. 指令函数不再写在虚拟机类里，而是写作一个函数对象
3. 位结构体（指令体）和指令函数（执行体），都有其抽象父类，其允许指令自己说明自己要接受的指令长度和标记号
4. 收集所有工具过程和固有过程，例如检查列表是否为空，并以工具函数表文件独立书写
5. 处理器内部状态将作为一个整体实例存在，像暂存器等都会放入VMState中，并且VMState将纯粹作为一个状态容器存在
6. 外设环境方面也会统一放入外设数据结构体中，以便自定义
## 改进方面
1. VM将可以由外部提供自定义指令，将可以把不同类型的数据做标记号（可序列化类型）
2. VM将重写线程模型，以更优雅的方式实现多线程
3. 将不限与IO接口的形式，而是提供更多途径例如内存钩子和IO钩子
4. 将考虑将环境以表的方式提供，以便动态扩充
5. 将考虑将VM作为模板类，由用户自己给定“处理器定义对象”和“外部环境对象”
6. 将使用C++11 14 17等更新版本的功能提供更优雅的外部交互方式
