#pragma once
#include "CentredWindow.h"
#include "TextureConfig.h"
#include "TerrainConfig.h"
#include "ISetupListener.h" 

class SetupWindow : public CentredWindow
{
public:
	SetupWindow(TextureConfig& textureConfig, TerrainConfig& terrainConfig);

	void drawContent() override;
	float getHeightScale() const;

	void setListener(ISetupListener* listener);

private:
	TextureConfig& m_textureConfig;
	TerrainConfig& m_terrainConfig;

	ISetupListener* m_listener; 
	float heightScale;
};