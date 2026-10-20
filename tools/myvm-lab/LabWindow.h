#pragma once

#include "InstructionView.h"
#include "LabSession.h"

#include <string>
#include <vector>

class LabWindow final
{
public:
    LabWindow();
    void draw();

private:
    void drawToolbar();
    void drawEditorPanel();
    void drawInstructionPanel();
    void drawRegisterPanel();
    void drawMemoryPanel();
    void drawTracePanel();
    void assembleSource();
    void openSource();
    void saveSource();
    void resetSession();
    void stepSession();
    void setDiagnostic(const myvm::lab::AssemblyDiagnostic& diagnostic);
    std::string memoryAddress(std::uint64_t address) const;

    myvm::lab::LabSession session_;
    std::vector<myvm::lab::InstructionRow> instructions_;
    std::string source_;
    std::string sourcePath_ = "program.asm";
    std::string message_;
    bool hasError_ = false;
    bool running_ = false;
    int stepsPerFrame_ = 32;
    int memoryOffset_ = 0;
};
