#include "VisualController.h"

VisualController::VisualController()
	: ToolController(),
	toolWindow(nullptr)
{}

VisualController& VisualController::getInstance()
{
	static VisualController instance;
	return instance;
}

void VisualController::setActiveTool(BaseTool* tool)
{
	this->toolWindow->setCurrentTool(tool);
}

BaseTool* VisualController::getActiveTool() const
{
    return this->toolWindow->getCurrentTool();
}

void VisualController::init(ToolWindow* window)
{
	toolWindow = window;
}
