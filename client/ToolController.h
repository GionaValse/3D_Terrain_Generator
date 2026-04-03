#pragma once

#include "IToolListener.h"

#include "BaseTool.h"
#include "ToolWindow.h"

class ToolController : public IToolListener
{
public:
	ToolController() 
		: currentTool(nullptr), 
		toolWindow(nullptr) 
	{}

	virtual void init(ToolWindow* window) = 0;
	virtual void free() const = 0;

protected:
	BaseTool* currentTool;
	ToolWindow* toolWindow;
};
