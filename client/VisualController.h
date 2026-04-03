#pragma once

#include "ToolController.h"

#include "ToolWindow.h"

class VisualController : public ToolController
{
public:
	static VisualController& getInstance();

	void init(ToolWindow* window) override;
	void free() const override;

	BaseTool* getActiveTool() const override;
	void onToolSelected(BaseTool* tool) override;

private:
	VisualController();

	VisualController(const VisualController&) = delete;
	VisualController& operator=(const VisualController&) = delete;

	ToolWindow* toolWindow;
};
