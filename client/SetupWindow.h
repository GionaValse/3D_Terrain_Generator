#pragma once

#include "CentredWindow.h"
#include "TerrainConfig.hpp"

class SetupWindow : public CentredWindow
{
public:
	SetupWindow();

	void drawContent() override;

	terrain::TerrainConfig getTerrainConfiguartion() const;

	float getHeightScale() const;

	bool checkAndResetTrigger();

private:
	terrain::TerrainConfig config;
	float heightScale;
	bool triggerGeneration;
};
