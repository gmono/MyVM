#include "LabWindow.h"

#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>

void LabWindow::drawEditorPanel()
{
    ImGui::TextUnformatted("汇编源码");
    ImGui::BeginChild("source-editor", ImVec2(0, 0), ImGuiChildFlags_Borders);
    ImGui::InputTextMultiline("##source-editor", &source_, ImVec2(-FLT_MIN, -FLT_MIN));
    ImGui::EndChild();
}
