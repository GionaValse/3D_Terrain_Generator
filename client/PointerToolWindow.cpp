#include "PointerToolWindow.h"

PointerToolWindow::PointerToolWindow()
	: ToolWindow("Pointer Tool", AnchorSide::RIGHT, ImVec2(0, 100), true, false)
{}

PointerToolWindow& PointerToolWindow::getInstance()
{
	static PointerToolWindow instance;
    return instance;
}

void PointerToolWindow::init(std::vector<std::vector<BaseTool*>> toolGroups)
{
	if (toolGroups.empty() || toolGroups[0].empty())
	{
		this->currentTool = nullptr;
		return;
	}

	this->tools = toolGroups;
	this->currentTool = toolGroups[0][0];
}
