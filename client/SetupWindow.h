#pragma once

#include "BaseWindow.h"
#include "TerrainConfig.hpp"

class SetupWindow : public BaseWindow
{
public:
	SetupWindow();
	
	void basePosition() override;

	void drawContent() override;

	terrain::TerrainConfig getTerrainConfiguartion() const;

	float getHeightScale() const;

	bool checkAndResetTrigger();

private:
	terrain::TerrainConfig config;
	float heightScale;
	bool triggerGeneration;
};
