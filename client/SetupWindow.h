#pragma once

#include "CentredWindow.h"
#include "ConfigController.h"

class SetupWindow : public CentredWindow
{
public:
	SetupWindow();

	void drawContent() override;

	float getHeightScale() const;

	bool checkAndResetTrigger();

private:
	float heightScale;
	bool triggerGeneration;
};
