#include "PointerToolWindow.h"

PointerToolWindow::PointerToolWindow()
	: ToolWindow("Pointer Tool", AnchorSide::RIGHT, ImVec2(0, 100), true, false)
{}

PointerToolWindow::~PointerToolWindow()
{}

void PointerToolWindow::init(std::vector<std::vector<BaseTool*>> toolGroups)
{
	this->tools = toolGroups;
}
