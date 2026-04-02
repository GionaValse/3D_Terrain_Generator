#pragma once

#include <vector>

#include "BaseTool.h"

class ToolController
{
public:
	ToolController() {}

	virtual void setActiveTool(BaseTool* tool) = 0;

	virtual BaseTool* getActiveTool() const = 0;
};
