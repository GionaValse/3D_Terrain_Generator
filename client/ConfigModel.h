#pragma once
#include <vector>
#include "TerrainConfig.h"
#include "TextureConfig.h"

class ConfigModel
{
public:
	ConfigModel() = default;

	TerrainConfig& getActiveTerrainConfig()
	{
		return m_terrainConfig;
	}

	TextureConfig& getActiveTextureConfig()
	{
		return m_textureConfig;
	}

	std::vector<float>& getHeightMapImage()
	{
		return m_heightMapImage;
	}

	void setHeightMapImage(const std::vector<float>& image)
	{
		m_heightMapImage = image;
	}

private:
	TerrainConfig m_terrainConfig;
	TextureConfig m_textureConfig;

	std::vector<float> m_heightMapImage;
};
