#pragma once
#include "engine.h"
#include "TerrainConfig.h"
#include "TextureConfig.h"
#include <vector>

class TerrainModel
{
public:
	TerrainModel(TerrainConfig terrainConfig, TextureConfig textureConfig);
	~TerrainModel();

	Eng::Mesh*          getTerrainMesh() const;
	Eng::Texture*       getHeightMapTexture() const;
	std::vector<float>& getTerrainImage();
	TerrainConfig       getTerrainConfig() const;
	TextureConfig       getTextureConfig() const;

private:
	void generate();

	TerrainConfig       terrainConfig;
	TextureConfig       textureConfig;
	Eng::Mesh*          terrainMesh;
	Eng::Texture*       heightMapTexture;
	std::vector<float>  terrainImage;
};
