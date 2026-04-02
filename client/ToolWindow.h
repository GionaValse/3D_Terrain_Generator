#pragma once

#include "AnchorWindow.h"

#include <vector>

#include "BaseTool.h"

class ToolWindow : public AnchorWindow
{
public:
    ToolWindow(
        const std::string& name = "",
        AnchorSide side = AnchorSide::RIGHT,
        ImVec2 offset = ImVec2(0, 0),
        bool startVisible = true,
        bool cancelable = false,
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration
    );

	virtual ~ToolWindow() = default;

	virtual void init(std::vector<std::vector<BaseTool*>> toolGroups) = 0;

	virtual void onToolSelected(BaseTool* tool);

    void drawContent() override;

    void clearTools();

    void setCurrentTool(BaseTool* tool);

    BaseTool* getCurrentTool() const;

protected:
    std::vector<std::vector<BaseTool*>> tools;
	BaseTool* currentTool;

    void renderToolButton(BaseTool* tool, ImVec2 iconSize);
};
