#pragma once

#include "AnchorWindow.h"

#include "BrushModel.h"

class BrushToolWindow : public AnchorWindow
{
public:
	BrushToolWindow(std::vector<BrushModel*> brushTools = {});

	~BrushToolWindow();

	void drawContent() override;

	BrushModel* getCurrentTool() const;

private:
	BrushModel* currentTool;
	std::vector<BrushModel*> brushes;

	void init(std::vector<BrushModel*> brushTools);
	void renderToolButton(BrushModel* brush, ImVec2 iconSize);
};
