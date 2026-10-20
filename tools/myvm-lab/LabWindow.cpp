#include "LabWindow.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>

LabWindow::LabWindow()
    : source_(
        ".code\n"
        "start:\n"
        "    ldl #d40\n"
        "    leareg r5, t0\n"
        "    save t0\n"
        "    ldl #d2\n"
        "    add t0\n"
        "    pause\n")
{
    assembleSource();
}

void LabWindow::draw()
{
    if (running_ && session_.hasProgram())
    {
        myvm::lab::AssemblyDiagnostic diagnostic;
        const auto executed = session_.run(static_cast<std::size_t>(stepsPerFrame_), diagnostic);
        if (!diagnostic.message.empty())
        {
            setDiagnostic(diagnostic);
            running_ = false;
        }
        else if (executed < static_cast<std::size_t>(stepsPerFrame_) ||
                 (!session_.trace().empty() && session_.trace().back().opcode == 0x0c))
            running_ = false;
    }

    ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, ImGuiCond_Always);
    ImGui::Begin("MyVM Lab", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

    drawToolbar();
    ImGui::Separator();

    const float availableHeight = ImGui::GetContentRegionAvail().y;
    const float lowerHeight = std::clamp(availableHeight * 0.32f, 190.0f, 340.0f);
    const float upperHeight = std::max(availableHeight - lowerHeight - 12.0f, 260.0f);
    ImGui::BeginChild("upper-workspace", ImVec2(0, upperHeight));
    if (ImGui::BeginTable("workspace", 3,
        ImGuiTableFlags_Resizable | ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_SizingStretchProp,
        ImVec2(0, 0)))
    {
        ImGui::TableSetupColumn("源代码", ImGuiTableColumnFlags_WidthStretch, 0.32f);
        ImGui::TableSetupColumn("指令与控制流", ImGuiTableColumnFlags_WidthStretch, 0.43f);
        ImGui::TableSetupColumn("处理器状态", ImGuiTableColumnFlags_WidthStretch, 0.25f);
        ImGui::TableNextRow();
        ImGui::TableNextColumn(); drawEditorPanel();
        ImGui::TableNextColumn(); drawInstructionPanel();
        ImGui::TableNextColumn(); drawRegisterPanel();
        ImGui::EndTable();
    }
    ImGui::EndChild();

    if (ImGui::BeginTabBar("lower-panels"))
    {
        if (ImGui::BeginTabItem("内存与栈"))
        {
            drawMemoryPanel();
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("执行历史"))
        {
            drawTracePanel();
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (!message_.empty())
    {
        ImGui::Separator();
        ImGui::TextColored(hasError_ ? ImVec4(1.0f, 0.40f, 0.34f, 1.0f) :
                                      ImVec4(0.40f, 0.82f, 0.64f, 1.0f), "%s", message_.c_str());
    }
    ImGui::End();
}

void LabWindow::drawToolbar()
{
    ImGui::SetNextItemWidth(260.0f);
    ImGui::InputText("##source-path", &sourcePath_);
    ImGui::SameLine();
    if (ImGui::Button("打开")) openSource();
    ImGui::SameLine();
    if (ImGui::Button("保存")) saveSource();
    ImGui::SameLine();
    if (ImGui::Button("汇编并加载")) assembleSource();
    ImGui::SameLine();
    ImGui::BeginDisabled(!session_.hasProgram());
    if (ImGui::Button("复位")) resetSession();
    ImGui::SameLine();
    if (ImGui::Button("单步")) stepSession();
    ImGui::SameLine();
    if (ImGui::Button(running_ ? "暂停" : "运行")) running_ = !running_;
    ImGui::EndDisabled();
    ImGui::SameLine();
    ImGui::SetNextItemWidth(140.0f);
    ImGui::SliderInt("每帧指令数", &stepsPerFrame_, 1, 256);

    if (session_.hasProgram())
    {
        ImGui::SameLine();
        ImGui::TextDisabled("%zu 字节 · %zu 条记录", session_.codeSize(), session_.trace().size());
    }
}

void LabWindow::assembleSource()
{
    myvm::lab::AssemblyDiagnostic diagnostic;
    running_ = false;
    if (!session_.assemble(source_, diagnostic))
    {
        setDiagnostic(diagnostic);
        return;
    }
    instructions_ = myvm::lab::decodeProgram(session_.bytecode());
    message_ = "汇编成功，VM 已复位并准备执行";
    hasError_ = false;
}

void LabWindow::openSource()
{
    std::ifstream input(sourcePath_);
    if (!input)
    {
        message_ = "无法打开文件：" + sourcePath_;
        hasError_ = true;
        return;
    }
    source_.assign(std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>());
    message_ = "已载入 " + sourcePath_;
    hasError_ = false;
}

void LabWindow::saveSource()
{
    std::ofstream output(sourcePath_);
    if (!output || !(output << source_))
    {
        message_ = "无法写入文件：" + sourcePath_;
        hasError_ = true;
        return;
    }
    message_ = "已保存 " + sourcePath_;
    hasError_ = false;
}

void LabWindow::resetSession()
{
    myvm::lab::AssemblyDiagnostic diagnostic;
    running_ = false;
    if (!session_.reset(diagnostic)) setDiagnostic(diagnostic);
    else
    {
        message_ = "VM 状态已复位";
        hasError_ = false;
    }
}

void LabWindow::stepSession()
{
    myvm::lab::AssemblyDiagnostic diagnostic;
    running_ = false;
    if (!session_.step(diagnostic)) setDiagnostic(diagnostic);
    else
    {
        message_.clear();
        hasError_ = false;
    }
}

void LabWindow::setDiagnostic(const myvm::lab::AssemblyDiagnostic& diagnostic)
{
    std::ostringstream text;
    if (diagnostic.line != 0)
        text << diagnostic.source << ':' << diagnostic.line << ':' << diagnostic.column << ": ";
    else if (!diagnostic.source.empty())
        text << diagnostic.source << ": ";
    text << diagnostic.message;
    message_ = text.str();
    hasError_ = true;
}

std::string LabWindow::memoryAddress(std::uint64_t address) const
{
    if (session_.memory().empty()) return "-";
    const auto base = reinterpret_cast<std::uintptr_t>(session_.memory().data());
    if (address >= base && address - base <= session_.memory().size())
        return "MEM+" + std::to_string(address - base);
    std::ostringstream text;
    text << "0x" << std::hex << std::uppercase << address;
    return text.str();
}
