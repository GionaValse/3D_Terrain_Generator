#pragma once
#include "CentredWindow.h"
#include "TextureConfig.h"
#include "TerrainConfig.h"

class SetupWindow : public CentredWindow
{
public:
	SetupWindow(TextureConfig& textureConfig, TerrainConfig& terrainConfig);

	void drawContent() override;

	float getHeightScale() const;
	bool checkAndResetTrigger();

private:
	TextureConfig& m_textureConfig;
	TerrainConfig& m_terrainConfig;

	float heightScale;
	bool triggerGeneration;
};