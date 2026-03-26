#pragma once

#include "AnchorWindow.h"

#include "ViewToolModel.h"

class ViewToolWindow : public AnchorWindow
{
public:
	ViewToolWindow(std::vector<ViewToolModel*> viewTools = {});

	~ViewToolWindow();

	void drawContent() override;

	ViewToolModel* getCurrentTool() const;

	void setCurrentTool(unsigned int index);

private:
	ViewToolModel* currentTool;
	std::vector<ViewToolModel*> views;

	void renderToolButton(ViewToolModel* view, ImVec2 iconSize);
};
