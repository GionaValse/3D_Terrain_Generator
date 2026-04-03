#pragma once

#include "ToolWindow.h"

class VisualToolWindow : public ToolWindow
{
public:
	static VisualToolWindow& getInstance();

	void init(std::vector<std::vector<BaseTool*>> toolGroups) override;

private:
	VisualToolWindow();

	VisualToolWindow(const VisualToolWindow&) = delete;
	void operator=(const VisualToolWindow&) = delete;
};
