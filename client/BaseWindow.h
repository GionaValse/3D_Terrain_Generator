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
    BaseWindow(
        const std::string& name = "",
        bool startVisible = true,
        bool cancelable = true,
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_None
    );

    virtual ~BaseWindow() = default;

    void setNextWindowPos(float x, float y, ImGuiCond cond = ImGuiCond_FirstUseEver);
    void setNextWindowSize(float w, float h);

    virtual void basePosition();
    virtual void render();

    virtual void drawContent() = 0;

    bool isOpen() const;

    void setVisible(bool visible);
    void setFlags(ImGuiWindowFlags newFlags);
};
