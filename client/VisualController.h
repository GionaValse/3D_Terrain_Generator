#pragma once

#include "ToolController.h"

#include "ToolWindow.h"

class VisualController : public ToolController
{
public:
	static VisualController& getInstance();

	void setActiveTool(BaseTool* tool) override;

	BaseTool* getActiveTool() const override;

	void init(ToolWindow* window);

private:
	VisualController();

	VisualController(const VisualController&) = delete;
	VisualController& operator=(const VisualController&) = delete;

	ToolWindow* toolWindow;
};
