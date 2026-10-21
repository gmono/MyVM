# 双层汇编转译器

转译器实现“源汇编 → MyVM 汇编”的第一层，随后由 `myvm-asm` 完成“MyVM 汇编 → 字节码”的第二层。

```asm
start:
    mov r5, 40
    add r5, 2
    cmp r5, 42
    je done
    halt
done:
    halt
```

```sh
build/myvm-translate source.asm translated.myvm.asm
build/myvm-asm translated.myvm.asm -o program.bin
```

当前源语言支持 `mov`、`add`、`sub`、`mul`、`div`、`and`、`or`、`xor`、`shl`、`shr`、`inc`、`not`、`cmp`、`jmp`、`je`、`jg`、`jl`、`push`、`pop`、`call`、`ret` 和 `halt`。每条源指令都会输出 `; @macro line=... op=...` 标记，便于教学工具把多个 micro-op 映射回一条上层指令。它是可扩展的教育型前端，不宣称兼容完整 x86/ARM 语法。

## Zig freestanding Intel 汇编

```sh
zig build-obj zig/demo.zig -target x86_64-freestanding -O ReleaseSmall \
  -femit-asm=/tmp/myvm-zig.s -fno-stack-check -fno-stack-protector -fstrip
build/myvm-translate --zig-intel /tmp/myvm-zig.s /tmp/myvm-zig.myvm.asm
build/myvm-asm /tmp/myvm-zig.myvm.asm -o /tmp/myvm-zig.bin
```

CMake 也提供 `MYVM_BUILD_ZIG_DEMO=ON` 和 `myvm-zig-demo` 目标。当前 Zig 后端支持 Zig 基础函数生成的 `mov`、`lea [reg + reg]`、栈帧元指令和 `ret`，遇到未支持的指令会报告错误，不静默生成错误字节码。
