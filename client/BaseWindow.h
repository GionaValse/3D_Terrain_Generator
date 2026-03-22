#pragma once

#include "imgui.h"
#include <string>

class BaseWindow {
protected:
    std::string title;
    bool isVisible;
    bool cancelable;
    ImGuiWindowFlags flags;
    ImVec2 defaultPos;
    ImVec2 defaultSize;
    ImGuiCond posCond;

public:
    BaseWindow(const std::string& name = "",
        bool startVisible = true,
        bool cancelable = true,
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None)
        : title(name),
        isVisible(startVisible),
        cancelable(cancelable),
        flags(windowFlags),
        defaultPos(-1.0f, -1.0f),
        defaultSize(-1.0f, -1.0f),
        posCond(ImGuiCond_FirstUseEver)
    {}

    virtual ~BaseWindow() = default;

    void setNextWindowPos(float x, float y, ImGuiCond cond = ImGuiCond_FirstUseEver) {
        defaultPos = ImVec2(x, y);
        posCond = cond;
    }

    void setNextWindowSize(float w, float h) {
        defaultSize = ImVec2(w, h);
    }

    virtual void basePosition()
    {
        if (defaultPos.x != -1.0f) {
            ImGui::SetNextWindowPos(defaultPos, posCond);
        }

        if (defaultSize.x != -1.0f) {
            ImGui::SetNextWindowSize(defaultSize, ImGuiCond_FirstUseEver);
        }
    }

    virtual void render()
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

    virtual void drawContent() = 0;

    void setVisible(bool visible) { isVisible = visible; }
    bool isOpen() const { return isVisible; }
    void setFlags(ImGuiWindowFlags newFlags) { flags = newFlags; }
};
