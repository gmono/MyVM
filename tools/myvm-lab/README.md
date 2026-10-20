# MyVM Lab

MyVM Lab 是直接驱动 MyVM C++ VM 的底层教学工作台，图形界面使用 Dear ImGui 原生 C++ API、SDL2 和 OpenGL3。

## 构建

在仓库根目录运行：

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DMYVM_BUILD_LAB=ON
cmake --build build --target myvm-lab
```

若机器上安装了多个 SDL2 CMake package，而选中的 package 不提供导入 target，可使用 `-DCMAKE_DISABLE_FIND_PACKAGE_SDL2=TRUE` 强制通过 pkg-config 查找 SDL2。

依赖 CMake 3.20+、C++23 编译器、SDL2/OpenGL 开发包和 Git。首次配置会从网络获取 ANTLR4 runtime 与 Dear ImGui 1.92.9b。启动：

```sh
./build/myvm-lab
```

界面会尝试加载系统中常见的 Noto Sans CJK、思源黑体、文泉驿、微软雅黑或苹方字体，以显示中文标签。

## 使用

左侧编辑或打开 `.asm` 文件，点击“汇编并加载”创建 VM。中央指令流显示 8 字节机器指令及操作数解释，当前 PC 所在行会高亮。右侧显示寄存器、数据暂存器和已初始化地址暂存器；“内存与栈”页显示程序字节、内存和栈顶，“执行历史”页记录 PC、操作码及 `currdata` 前后值。

“单步”执行一条 VM 操作；“运行”按每帧指令数推进，并在 `pause` 或错误处暂停。“复位”从当前字节码重新创建 VM。汇编错误会显示源位置，并保留此前已加载的 VM。

内置示例依次设置 `currdata = 40`、建立到 R5 的地址暂存器、保存 40、加 2 并写回 R5，最后在 `pause` 停下。可逐步观察 PC、T0 和 R5 的变化。
