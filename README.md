# MyVM

MyVM 是用于研究和实验的 C++23 虚拟机、定长字节码汇编器和可视化教学工作台。VM 保留 8 字节指令、显式状态、寄存器/暂存器、中断、端口和同步步进等设计。

## 构建与测试

需要 CMake 3.20+、支持 C++23 的编译器和 Git。CMake 从固定版本获取 ANTLR4 4.13.2 C++ runtime；首次配置需要网络。ANTLR generator jar 位于 `tools/antlr/`。

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

默认构建 VM 静态库 `vmlib`、ANTLR 汇编器、`myvm-asm` 命令行工具和 GUI 无关的教学模型。`BUILD_SHARED_LIBS=ON` 可构建共享 VM 库。安装库、CMake package 和汇编器：

```sh
cmake --install build --prefix ./install
```

下游 CMake 项目可通过 `find_package(MyVM CONFIG REQUIRED)` 和 `MyVM::vmlib` 链接 VM 核心。

## MyVM Lab

启用 Dear ImGui 桌面教学工作台需要 SDL2 和 OpenGL 开发包；CMake 会获取固定版本 Dear ImGui 1.92.9b。

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DMYVM_BUILD_LAB=ON
cmake --build build --target myvm-lab
./build/myvm-lab
```

工作台直接使用本仓库的 C++ 汇编器和 VM，支持编辑/保存汇编源码、查看汇编诊断、单步与限量连续执行，并观察当前指令、PC、通用寄存器、`currdata`、地址暂存器、内存、栈和执行历史。内置示例会把 40 加 2 写入 R5，并在 `pause` 指令处停下。详细操作见 [`tools/myvm-lab/README.md`](tools/myvm-lab/README.md)。

汇编语法、指令表和示例见[汇编语言说明](汇编语言.md)。命令行汇编：

```sh
build/myvm-asm program.asm -o program.bin
```

双层汇编转译器可把教育型源汇编先降低为 MyVM 汇编，再交给同一个汇编器生成字节码：

```sh
build/myvm-translate source.asm translated.myvm.asm
build/myvm-asm translated.myvm.asm -o program.bin
```

当前支持 `mov`、`add`、`sub`、`mul`、`div`、`and`、`or`、`xor`、`shl`、`shr`、`inc`、`not`、`cmp`、`jmp`、`je`、`jg`、`jl`、`push`、`pop`、`call`、`ret` 和 `halt`。底层指令作为 micro-op；每条上层源指令可展开为多个 micro-op，并保留源行映射标记。转译层只负责 lowering，最终编码和诊断仍由 MyVM ANTLR 汇编器负责。

仓库包含 Zig freestanding Intel 汇编的实验性入口，但它目前只是转译器基础设施验证，不能视为完整 x86-64 支持。完整指令族、寄存器、RFLAGS、寻址和 SIMD 的目标见 [x86-64 支持矩阵](x86-64支持矩阵.md)。

仓库现在还集成了 Zydis 4.1.1 的完整 x86-64 解码层。启用 `MYVM_BUILD_X86=ON`（默认开启）后可使用：

```sh
build/myvm-x86-decode raw.bin
```

该层可以识别完整 x86-64 编码并保留原始字节、长度、助记符、寄存器、立即数和内存操作数；语义 lowering 仍按 [x86-64 支持矩阵](x86-64支持矩阵.md) 分阶段放行，未实现语义的指令不会被静默转换。

设计边界和仍待补足的测试见[设计与升级方案](设计与升级方案.md)及[升级评估](升级评估.md)。
# 目的说明
MyVM 是用于研究和实验的虚拟机底层库与汇编器。设计重点是方便调试、观察 VM 状态和实验中断、寄存器、端口及执行控制，不以极限性能为首要目标。VM 同时提供同步定步执行和异步连续执行。

# 重启计划
详细设计边界、已完成修复和后续阶段见[设计与升级方案](设计与升级方案.md)。本次维护保留固定 8 字节指令、显式 VMState、可观察状态、异步执行、中断和外部端口等原始设计方向。
