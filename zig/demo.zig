// 由 Zig 编译器生成 x86_64 freestanding Intel 汇编，再交给 myvm-translate。
pub export fn add(a: u64, b: u64) u64 {
    return a + b;
}
