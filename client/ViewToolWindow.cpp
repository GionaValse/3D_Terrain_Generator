#include "ViewToolWindow.h"

ViewToolWindow::ViewToolWindow(std::vector<ViewToolModel*> viewTools)
    : AnchorWindow("viewToolWindow", AnchorSide::RIGHT, ImVec2(0.0f, -100.0f)),
    currentTool(nullptr),
    views(viewTools)
{}

ViewToolWindow::~ViewToolWindow()
{
    for (auto view : views)
        delete view;
}

void ViewToolWindow::drawContent()
{
    ImVec2 iconSize(18, 18);

    for (int i = 0; i < views.size(); i++)
    {
        if (i != 0)
            ImGui::Dummy(ImVec2(0, 5));
        renderToolButton(views[i], iconSize);
    }
}

ViewToolModel* ViewToolWindow::getCurrentTool() const
{
	return currentTool;
}

void ViewToolWindow::setCurrentTool(unsigned int index)
{
    if (index >= views.size()) return;
    currentTool = views[index];
}

void ViewToolWindow::renderToolButton(ViewToolModel * view, ImVec2 iconSize)
{
    if (!view) return;
    bool isViewActive = (currentTool == view);

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(6.0f, 6.0f));

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 6.0f);

    if (isViewActive) {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 2.5f);
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 0.8f, 0.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.25f, 0.25f, 0.25f, 1.0f));
    }
    else
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
    }

    ImGui::PushID(view->getName().c_str());

    ImVec4 tint = isViewActive ? ImVec4(1.0f, 0.85f, 0.4f, 1.0f) : ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

    if (ImGui::ImageButton("##btn", (ImTextureID)(intptr_t)view->getIconId(),
        iconSize, ImVec2(0, 1), ImVec2(1, 0),
        ImVec4(0, 0, 0, 0), tint))
    {
        currentTool = view;
    }

    ImGui::PopID();

    ImGui::PopStyleVar(2);
    if (isViewActive) {
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
    else {
        ImGui::PopStyleVar();
    }
}
