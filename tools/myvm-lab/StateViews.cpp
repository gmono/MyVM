#include "LabWindow.h"

#include <imgui.h>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace
{
std::string hexValue(std::uint64_t value, int width = 0)
{
    std::ostringstream text;
    text << "0x" << std::hex << std::uppercase << std::setfill('0') << std::setw(width) << value;
    return text.str();
}

const char* registerName(std::size_t index)
{
    static constexpr const char* names[] = {"PC 程序计数器", "IVT 中断表", "IVT 长度", "SP 栈指针", "FLAGS 标志"};
    return index < std::size(names) ? names[index] : "通用寄存器";
}
}

void LabWindow::drawInstructionPanel()
{
    ImGui::TextUnformatted("指令流");
    ImGui::BeginChild("instruction-list", ImVec2(0, 0), ImGuiChildFlags_Borders);
    if (ImGui::BeginTable("instructions", 4,
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_SizingStretchProp |
        ImGuiTableFlags_ScrollY))
    {
        ImGui::TableSetupColumn("偏移", ImGuiTableColumnFlags_WidthFixed, 76.0f);
        ImGui::TableSetupColumn("机器码", ImGuiTableColumnFlags_WidthFixed, 160.0f);
        ImGui::TableSetupColumn("指令", ImGuiTableColumnFlags_WidthFixed, 118.0f);
        ImGui::TableSetupColumn("语义", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableHeadersRow();

        std::uint64_t pc = std::numeric_limits<std::uint64_t>::max();
        if (session_.hasProgram()) pc = session_.inspect().state.regs[0];
        ImGuiListClipper clipper;
        clipper.Begin(static_cast<int>(instructions_.size()));
        while (clipper.Step())
        {
            for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
            {
                const auto& row = instructions_[static_cast<std::size_t>(i)];
                const bool current = pc >= row.offset && pc < row.offset + row.size;
                ImGui::TableNextRow();
                if (current)
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(39, 86, 93, 255));
                ImGui::TableNextColumn(); ImGui::Text("%06zX", row.offset);
                ImGui::TableNextColumn();
                std::ostringstream bytes;
                const auto count = std::min(row.size, session_.bytecode().size() - row.offset);
                for (std::size_t b = 0; b < count; ++b)
                {
                    if (b == 8 && row.hasAddressLiteral) bytes << "| ";
                    bytes << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                          << static_cast<unsigned>(session_.bytecode()[row.offset + b]) << ' ';
                }
                ImGui::TextUnformatted(bytes.str().c_str());
                ImGui::TableNextColumn();
                ImGui::Text("%s %s", row.mnemonic.c_str(), row.operands.c_str());
                ImGui::TableNextColumn(); ImGui::TextWrapped("%s", row.meaning.c_str());
                if (current) ImGui::SetScrollHereY(0.5f);
            }
        }
        ImGui::EndTable();
    }
    ImGui::EndChild();
}

void LabWindow::drawRegisterPanel()
{
    ImGui::TextUnformatted("寄存器与暂存器");
    ImGui::BeginChild("registers", ImVec2(0, 0), ImGuiChildFlags_Borders);
    if (!session_.hasProgram())
    {
        ImGui::TextDisabled("汇编程序后显示状态");
        ImGui::EndChild();
        return;
    }

    const auto inspection = session_.inspect();
    for (std::size_t i = 0; i < inspection.state.regs.sum; ++i)
    {
        const auto value = inspection.state.regs[static_cast<dword>(i)];
        ImGui::Text("R%-2zu  %-18s", i, registerName(i));
        ImGui::SameLine();
        if (i == 0) ImGui::Text("%zu", static_cast<std::size_t>(value));
        else if (i == 3) ImGui::Text("%s", memoryAddress(value).c_str());
        else ImGui::Text("%s", hexValue(value, 16).c_str());
    }
    ImGui::Separator();
    ImGui::Text("currdata"); ImGui::SameLine(); ImGui::Text("%s", hexValue(inspection.currdata, 16).c_str());
    ImGui::Text("VM step：%llu", static_cast<unsigned long long>(inspection.state.nowstep));
    const auto flags = static_cast<unsigned>(inspection.state.regs[4] & 3);
    const char* relation = flags == 0 ? "小于" : flags == 1 ? "相等" : flags == 2 ? "大于" : "无效组合";
    ImGui::Text("比较结果：%s", relation);
    ImGui::Separator();
    ImGui::TextUnformatted("已初始化地址暂存器");
    if (inspection.temporaries.empty()) ImGui::TextDisabled("无");
    for (const auto& temporary : inspection.temporaries)
    {
        ImGui::Text("T%-5u %s", temporary.id, hexValue(temporary.value, 16).c_str());
        ImGui::SameLine();
        ImGui::TextDisabled("@ %s", memoryAddress(temporary.address).c_str());
    }
    ImGui::EndChild();
}

void LabWindow::drawMemoryPanel()
{
    if (!session_.hasProgram())
    {
        ImGui::TextDisabled("汇编程序后显示内存");
        return;
    }
    const auto& memory = session_.memory();
    const auto maxOffset = std::max(0, static_cast<int>(memory.size()) - 16);
    ImGui::SetNextItemWidth(300.0f);
    ImGui::SliderInt("内存偏移", &memoryOffset_, 0, maxOffset, "%06X");
    ImGui::SameLine();
    ImGui::TextDisabled("代码 %zu B · 内存 %zu B", session_.codeSize(), memory.size());

    const auto inspection = session_.inspect();
    const auto base = reinterpret_cast<std::uintptr_t>(memory.data());
    const auto stackAddress = static_cast<std::uintptr_t>(inspection.state.regs[3]);
    const std::size_t stackOffset = stackAddress >= base && stackAddress - base <= memory.size()
        ? stackAddress - base : memory.size();

    if (ImGui::BeginTable("memory-grid", 3,
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp, ImVec2(0, ImGui::GetContentRegionAvail().y * 0.65f)))
    {
        ImGui::TableSetupColumn("偏移", ImGuiTableColumnFlags_WidthFixed, 88.0f);
        ImGui::TableSetupColumn("字节", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("ASCII", ImGuiTableColumnFlags_WidthFixed, 140.0f);
        ImGui::TableHeadersRow();
        const std::size_t begin = static_cast<std::size_t>(memoryOffset_) & ~std::size_t{15};
        const std::size_t end = std::min(memory.size(), begin + 16 * 32);
        for (std::size_t offset = begin; offset < end; offset += 16)
        {
            ImGui::TableNextRow();
            if (offset <= stackOffset && stackOffset < offset + 16)
                ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, IM_COL32(101, 76, 40, 180));
            ImGui::TableNextColumn(); ImGui::Text("%06zX", offset);
            ImGui::TableNextColumn();
            std::ostringstream bytes;
            std::string ascii;
            for (std::size_t i = 0; i < 16 && offset + i < memory.size(); ++i)
            {
                bytes << std::hex << std::uppercase << std::setfill('0') << std::setw(2)
                      << static_cast<unsigned>(memory[offset + i]) << ' ';
                const auto c = memory[offset + i];
                ascii.push_back(c >= 32 && c < 127 ? static_cast<char>(c) : '.');
            }
            ImGui::TextUnformatted(bytes.str().c_str());
            ImGui::TableNextColumn(); ImGui::TextUnformatted(ascii.c_str());
        }
        ImGui::EndTable();
    }

    ImGui::Text("栈顶 SP：%s", memoryAddress(inspection.state.regs[3]).c_str());
    if (stackOffset < memory.size())
    {
        for (std::size_t i = 0, offset = stackOffset; i < 6 && offset + sizeof(qword) <= memory.size();
             ++i, offset += sizeof(qword))
        {
            qword value = 0;
            std::memcpy(&value, memory.data() + offset, sizeof(value));
            ImGui::Text("%s  %s", memoryAddress(base + offset).c_str(), hexValue(value, 16).c_str());
        }
    }
}

void LabWindow::drawTracePanel()
{
    const auto& trace = session_.trace();
    if (trace.empty())
    {
        ImGui::TextDisabled("执行后显示最近的指令变化");
        return;
    }
    if (ImGui::BeginTable("execution-trace", 6,
        ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersInnerH | ImGuiTableFlags_ScrollY |
        ImGuiTableFlags_SizingStretchProp))
    {
        ImGui::TableSetupColumn("序号", ImGuiTableColumnFlags_WidthFixed, 80.0f);
        ImGui::TableSetupColumn("PC", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableSetupColumn("操作码", ImGuiTableColumnFlags_WidthFixed, 90.0f);
        ImGui::TableSetupColumn("数据前", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("数据后", ImGuiTableColumnFlags_WidthStretch);
        ImGui::TableSetupColumn("PC 后", ImGuiTableColumnFlags_WidthFixed, 100.0f);
        ImGui::TableHeadersRow();
        const std::size_t first = trace.size() > 256 ? trace.size() - 256 : 0;
        for (std::size_t i = first; i < trace.size(); ++i)
        {
            const auto& entry = trace[i];
            ImGui::TableNextRow();
            ImGui::TableNextColumn(); ImGui::Text("%llu", static_cast<unsigned long long>(entry.sequence));
            ImGui::TableNextColumn(); ImGui::Text("%06llX", static_cast<unsigned long long>(entry.pcBefore));
            ImGui::TableNextColumn(); ImGui::Text("%02X", entry.opcode);
            ImGui::TableNextColumn(); ImGui::Text("%s", hexValue(entry.dataBefore, 16).c_str());
            ImGui::TableNextColumn(); ImGui::Text("%s", hexValue(entry.dataAfter, 16).c_str());
            ImGui::TableNextColumn(); ImGui::Text("%06llX", static_cast<unsigned long long>(entry.pcAfter));
        }
        ImGui::EndTable();
    }
}
