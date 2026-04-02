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
	if (toolGroups.empty() || toolGroups[0].empty())
	{
		this->currentTool = nullptr;
		return;
	}

	this->tools = toolGroups;
	this->currentTool = toolGroups[0][1];
}

void VisualToolWindow::onToolSelected(BaseTool* tool)
{
	ToolWindow::onToolSelected(tool);
	
	if (auto* visualTool = dynamic_cast<BaseVisualTool*>(tool))
	{
		visualTool->use();
	}
}
