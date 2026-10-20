# MyVM ImGui 教学工作台 Implementation Plan

> **For agentic workers:** 本计划由当前代理在同一会话内按步骤执行，不拆分子代理。步骤使用复选框追踪。

**Goal:** 增加原生 C++ Dear ImGui 教学工作台，使用真实 MyVM 汇编器和 VM 进行测试与可视化。

**Architecture:** `myvm-lab-core` 直接链接现有 C++23 汇编器和 VM；VM 提供停止态调试快照；SDL2/OpenGL 上运行 Dear ImGui 界面。GUI 由 CMake 选项启用，模型测试不依赖 GUI。

**Tech Stack:** C++23、CMake、Dear ImGui v1.92.9b、SDL2、OpenGL3、CTest。

---

### Task 1: VM 调试快照

**Files:**
- Modify: `vmlib/mvm.h`
- Modify: `vmlib/mvm.cpp`
- Test: `test/lab_model_tests.cpp`
- Modify: `CMakeLists.txt`

- [x] **Step 1: 先写快照测试**，运行 3 条指令建立临时寄存器引用并存入值，断言快照返回 PC、`currdata`、临时寄存器地址和值。
- [x] **Step 2: 运行目标测试确认失败**：`cmake --build build --target myvm-lab-tests` 因 `MVM::inspect()` 尚不存在而编译失败。
- [x] **Step 3: 增加停止态快照类型和 `MVM::inspect()`**，用 `executionlock` 的 try-lock 拒绝 VM 执行期间检查，在锁内复制状态并读取已初始化暂存器。
- [x] **Step 4: 重跑模型测试**：`VM debug inspection passed`。

### Task 2: GUI 无关的教学模型

**Files:**
- Create: `tools/myvm-lab/LabSession.h`
- Create: `tools/myvm-lab/LabSession.cpp`
- Create: `tools/myvm-lab/InstructionView.h`
- Create: `tools/myvm-lab/InstructionView.cpp`
- Test: `test/lab_model_tests.cpp`
- Modify: `CMakeLists.txt`

- [x] **Step 1: 先补模型测试**：测试汇编/诊断、会话保留、VM 单步状态、`leamem` 字面量解码、复位和内存快照。
- [x] **Step 2: 确认模型测试失败**：编译器因 `InstructionView.h` 尚不存在而失败。
- [x] **Step 3: 实现 `LabSession`**：汇编成功后建立稳定的程序+栈内存，配置 8 个通用寄存器，支持复位、单步及每次最多指定条数的同步运行；汇编失败不替换现有会话。
- [x] **Step 4: 实现指令行解码**：覆盖 opcode `0x00` 至 `0x16`，解释小端操作数，并将 `leamem` 后续字面量标为数据槽。
- [x] **Step 5: 运行模型和原有 CTest**：模型目标通过；全部 CTest 将在最终集成后统一重跑。

### Task 3: Dear ImGui 桌面程序

**Files:**
- Create: `tools/myvm-lab/main.cpp`
- Create: `tools/myvm-lab/LabWindow.h`
- Create: `tools/myvm-lab/LabWindow.cpp`
- Create: `tools/myvm-lab/Editor.cpp`
- Create: `tools/myvm-lab/StateViews.cpp`
- Modify: `CMakeLists.txt`

- [x] **Step 1: 固定 CMake GUI 选项和依赖**：`MYVM_BUILD_LAB` 默认关闭；开启后查找 SDL2/OpenGL，获取固定 Dear ImGui tag，编译 SDL2/OpenGL3 后端。
- [x] **Step 2: 创建 SDL2 + OpenGL3 + Dear ImGui 主循环**，窗口初始化失败时打印错误并返回非零。
- [x] **Step 3: 添加源码编辑、路径打开/保存、汇编诊断和演示程序**。
- [x] **Step 4: 添加解码列表、PC 高亮、寄存器/暂存器面板、内存/栈视图和执行历史**。
- [x] **Step 5: 实现单步、限量连续运行、暂停和复位，并在异常时暂停显示错误**。
- [x] **Step 6: 开启 GUI 构建并运行 `myvm-lab`，截图确认中文界面和下方面板；自动点击执行后 PC=48、R5=0x2A、在 `pause` 后停止。**

### Task 4: 文档和最终检查

**Files:**
- Modify: `README.md`
- Create: `tools/myvm-lab/README.md`

- [x] **Step 1: 写构建依赖、CMake 开关、启动命令和面板行为说明**。
- [x] **Step 2: 运行 Debug 构建、CTest、GUI 配置/构建、`git diff --check`**：GUI 开启时完整构建和 CTest 4/4 通过；关闭 GUI 的核心构建/CTest 4/4 通过；ASan/UBSan CTest 4/4 通过。
- [x] **Step 3: 检查全部变更和当前工作树，保留此前工作区已有改动。**
