#pragma once

#include "imgui.h"
#include <string>

class BaseWindow {
protected:
    std::string title;
    bool isVisible;

public:
    BaseWindow(const std::string& name) 
        : title(name), 
        isVisible(true)
    {}

    virtual ~BaseWindow() = default;

    void render() 
    {
        if (!isVisible) return;

        ImGui::Begin(title.c_str(), &isVisible);
        drawContent();
        ImGui::End();
    }

    virtual void drawContent() = 0;

    void setVisible(bool visible) { isVisible = visible; }
    bool isOpen() const { return isVisible; }
};
