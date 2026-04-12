#include "TerrainModel.h"

#include <iostream>

#include "TerrainGenerator.h"
#include "TextureGenerator.h"
#include "ImageExporter.hpp"

TerrainModel::TerrainModel(TerrainConfig terrainConfig, TextureConfig textureConfig)
	: terrainMesh(nullptr),
	heightMapTexture(nullptr),
	terrainConfig(terrainConfig),
	textureConfig(textureConfig)
{
	generate();
}

TerrainModel::~TerrainModel()
{
	if (terrainMesh)
	{
		Eng::Node* root = Eng::Base::getInstance().getSceneGraphInstance();
		root->removeChild(this->terrainMesh);
		delete this->terrainMesh;
		this->terrainMesh = nullptr;
	}
}

void TerrainModel::loadOnScene()
{
	if (!terrainMesh) return;

	heightMapTexture = new Eng::Texture("TerrainHeightMap", textureConfig.size, textureConfig.size, terrainImage);

	Eng::Material* material = new Eng::Material("TerrainMaterial");
	material->setSpecular(glm::vec4(0.0f));
	material->setTexture(heightMapTexture);

	terrainMesh->setMaterial(material);

	Eng::Shader* shader = Eng::Shader::getCurrentInstance();
	if (shader) {
		int heightScaleLoc = shader->getParamLocation("heightScale");
		shader->setFloat(heightScaleLoc, terrainConfig.heightScale);
	}

	Eng::Node* root = Eng::Base::getInstance().getSceneGraphInstance();
	root->addChild(terrainMesh);
}

Eng::Mesh* TerrainModel::getTerrainMesh() const
{
	return this->terrainMesh;
}

Eng::Texture* TerrainModel::getHeightMapTexture() const
{
	return this->heightMapTexture;
}

std::vector<float>& TerrainModel::getTerrainImage()
{
	return this->terrainImage;
}

TerrainConfig TerrainModel::getTerrainConfig() const
{
	return this->terrainConfig;
}

TextureConfig TerrainModel::getTextureConfig() const
{
	return this->textureConfig;
}

void TerrainModel::generate()
{
	TerrainGenerator terrGenerator(terrainConfig);
	TextureGenerator textGenerator(textureConfig);

	terrainImage.clear();
	terrainImage = textGenerator.generate();
	std::string textName;

	if (!terrain::ImageExporter::saveEXR(terrainImage, textureConfig, textName))
	{
		std::cerr << "Unable to save the texture image" << std::endl;
		return;
	}

	terrainMesh = terrGenerator.generate();
	terrainMesh->setMatrix(glm::mat4(1.0f));
}
