#pragma once

#include <vector>

#include "TerrainConfig.h"
#include "TextureConfig.h"

class ConfigController
{
public:
	static ConfigController& getInstance()
	{ 
		static ConfigController instance;
		return instance;
	}

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
	ConfigController() = default; 

	ConfigController(const ConfigController&) = delete;
	ConfigController& operator=(const ConfigController&) = delete;

	TerrainConfig m_terrainConfig;
	TextureConfig m_textureConfig;

	std::vector<float> m_heightMapImage;
};