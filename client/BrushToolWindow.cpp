#include "BrushToolWindow.h"

BrushToolWindow::BrushToolWindow(std::vector<BrushModel*> brushTools)
	: AnchorWindow("BrushToolWindow", AnchorSide::RIGHT, ImVec2(0.0f, 100.0f)),
    currentTool(nullptr),
    brushes({})
{
    init(brushTools);
}

BrushToolWindow::~BrushToolWindow()
{
    for (auto brush : brushes)
        delete brush;
}

void BrushToolWindow::drawContent()
{
    ImVec2 iconSize(18, 18);

    renderToolButton(brushes[0], iconSize);

    if (brushes.empty()) return;

    ImGui::Dummy(ImVec2(0, 5));
    ImGui::Separator();

    for (int i = 1; i < brushes.size(); i++)
    {
        ImGui::Dummy(ImVec2(0, 5));
        renderToolButton(brushes[i], iconSize);
    }    
}

BrushModel* BrushToolWindow::getCurrentTool() const
{
    return currentTool;
}

void BrushToolWindow::init(std::vector<BrushModel*> brushTools)
{
    brushes.clear();
    BrushModel* arrowBrush = new BrushModel("ArrowBrush", "./res/cursor.png");

    brushes.push_back(arrowBrush);
    for (auto brush : brushTools)
        brushes.push_back(brush);

    currentTool = arrowBrush;
}

void BrushToolWindow::renderToolButton(BrushModel* brush, ImVec2 iconSize)
{
    if (!brush) return;
    bool isBrushActive = (currentTool == brush);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    if (isBrushActive) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    }
    else 
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    }

    ImGui::PushID(brush->getName().c_str());

    ImVec4 tint = isBrushActive ? ImVec4(1.0f, 0.85f, 0.4f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)brush->getIconId(),
        iconSize, ImVec2(0, 1), ImVec2(1, 0),
        ImVec4(0, 0, 0, 0), tint))
    {
        currentTool = brush;
    }

    ImGui::PopID();

    ImGui::PopStyleVar(2);
    if (isBrushActive) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
    else {
        ImGui::PopStyleVar();
    }
}
