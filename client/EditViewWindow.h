#pragma once

#include "BaseWindow.h"
#include "engine.h"

class EditViewWindow : public BaseWindow
{
public:
	EditViewWindow();

	void drawContent() override;

	Eng::Base& getEngineInstance();
};
