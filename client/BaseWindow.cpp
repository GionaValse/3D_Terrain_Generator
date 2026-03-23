#include "BaseWindow.h"

BaseWindow::BaseWindow(const std::string& name, bool startVisible, bool cancelable, ImGuiWindowFlags windowFlags)
	: title(name),
    isVisible(startVisible),
    cancelable(cancelable),
    flags(windowFlags),
    defaultPos(-1.0f, -1.0f),
    defaultSize(-1.0f, -1.0f),
    posCond(ImGuiCond_FirstUseEver)
{}

void BaseWindow::setNextWindowPos(float x, float y, ImGuiCond cond)
{
    defaultPos = ImVec2(x, y);
    posCond = cond;
}

void BaseWindow::setNextWindowSize(float w, float h)
{
    defaultSize = ImVec2(w, h);
}

void BaseWindow::basePosition()
{
    if (defaultPos.x != -1.0f) {
        ImGui::SetNextWindowPos(defaultPos, posCond);
    }

    if (defaultSize.x != -1.0f) {
        ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
    }
}

void BaseWindow::render()
{
    if (!isVisible) return;

    basePosition();

    if (!ImGui::Begin(title.c_str(), cancelable ? &isVisible : nullptr, flags)) {
        ImGui::End();
        return;
    }

    drawContent();

    ImGui::End();
}

bool BaseWindow::isOpen() const
{
    return isVisible;
}

void BaseWindow::setVisible(bool visible)
{
    isVisible = visible;
}

void BaseWindow::setFlags(ImGuiWindowFlags newFlags)
{
    flags = newFlags;
}
