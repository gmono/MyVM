# Zig freestanding 双层流水线

本目录验证 Zig 生成的 `x86_64-freestanding` Intel 汇编可以进入 MyVM 第二层转译器。当前后端只接收少量基础函数形态，属于实验性验证；它不是完整 x86-64 转译器。完整目标见仓库根目录的 `x86-64支持矩阵.md`。

```sh
zig build-obj zig/demo.zig \
  -target x86_64-freestanding -O ReleaseSmall \
  -femit-asm=/tmp/myvm-zig.s -fno-stack-check -fno-stack-protector -fstrip
build/myvm-translate --zig-intel /tmp/myvm-zig.s /tmp/myvm-zig.myvm.asm
build/myvm-asm /tmp/myvm-zig.myvm.asm -o /tmp/myvm-zig.bin
```

Zig ABI 寄存器映射暂定为：`rax→r5`、`rbx→r6`、`rcx→r7`、`rdx→r8`、`rdi→r9`、`rsi→r10`。这不是通用 x86 转译器；遇到未支持的指令会报告行号并停止，避免静默生成错误字节码。
