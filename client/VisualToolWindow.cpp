#include "VisualToolWindow.h"

#include "BaseVisualTool.h"

VisualToolWindow::VisualToolWindow()
	: ToolWindow("Visual Tool", AnchorSide::RIGHT, ImVec2(0, -100), true, false)
{}

VisualToolWindow& VisualToolWindow::getInstance()
{
	static VisualToolWindow instance;
	return instance;
}

void VisualToolWindow::init(std::vector<std::vector<BaseTool*>> toolGroups)
{
	this->tools = toolGroups;
}
