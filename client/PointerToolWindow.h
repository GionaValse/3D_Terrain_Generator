#pragma once

#include "ToolWindow.h"

class PointerToolWindow : public ToolWindow
{
public:
	static PointerToolWindow& getInstance();

	void init(std::vector<std::vector<BaseTool*>> toolGroups) override;

private:
	PointerToolWindow();

	PointerToolWindow(const PointerToolWindow&) = delete;
	void operator=(const PointerToolWindow&) = delete;
};
