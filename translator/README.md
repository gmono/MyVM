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

当前源语言支持 `mov`、`add`、`sub`、`mul`、`div`、`cmp`、`jmp`、`je`、`jg`、`jl`、`push`、`pop`、`call`、`ret` 和 `halt`。它是可扩展的教育型前端，不宣称兼容完整 x86/ARM 语法。
