#include "VisualController.h"

#include "BaseVisualTool.h"

VisualController::VisualController()
	: ToolController(),
	toolWindow(nullptr)
{}

VisualController& VisualController::getInstance()
{
	static VisualController instance;
	return instance;
}

BaseTool* VisualController::getActiveTool() const
{
    return this->currentTool;
}

void VisualController::onToolSelected(BaseTool* tool)
{
	this->currentTool = tool;

	if (auto visualTool = dynamic_cast<BaseVisualTool*>(this->currentTool))
	{
		visualTool->use();
	}
}

void VisualController::init(ToolWindow* window)
{
	this->toolWindow = window;

	if (this->toolWindow)
	{
		this->toolWindow->setListener(this);
	}
}

void VisualController::free() const
{}
