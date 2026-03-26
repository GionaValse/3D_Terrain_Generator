#include "BrushBrushModel.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

BrushBrushModel::BrushBrushModel(std::string name, std::string res)
	: BrushModel(name, res, BrushToolType::BRUSH),
	heightMap(nullptr)
{}

BrushBrushModel::~BrushBrushModel()
{}

void BrushBrushModel::use(glm::vec3 coords, terrain::TerrainConfig config, std::vector<float>& image)
{
	float terrainPhysicalSize = 512.0f;
	int imageResolution = config.size;

	float localX = coords.x + (terrainPhysicalSize / 2.0f);
	float localZ = coords.z + (terrainPhysicalSize / 2.0f);

	float u = localX / terrainPhysicalSize;
	float v = localZ / terrainPhysicalSize;

	int pixelX = static_cast<int>(u * imageResolution);
	int pixelY = static_cast<int>(v * imageResolution);

	if (pixelX < 0 || pixelX >= imageResolution || pixelY < 0 || pixelY >= imageResolution)
		return;

	int brushRadius = 15;
	float brushStrength = 0.005f;

	int startX = std::max(0, pixelX - brushRadius);
	int startY = std::max(0, pixelY - brushRadius);
	int endX = std::min(imageResolution - 1, pixelX + brushRadius);
	int endY = std::min(imageResolution - 1, pixelY + brushRadius);

	bool modified = false;

	for (int y = startY; y <= endY; ++y)
	{
		for (int x = startX; x <= endX; ++x)
		{
			float distX = (float)(x - pixelX);
			float distY = (float)(y - pixelY);
			float distance = std::sqrt(distX * distX + distY * distY);

			if (distance <= brushRadius)
			{
				float falloff = 1.0f - (distance / brushRadius);
				float raiseAmount = brushStrength * falloff;
				int index = (y * imageResolution + x) * 3;

				image[index + 0] = std::clamp(image[index + 0] + raiseAmount, 0.0f, 1.0f);
				image[index + 1] = std::clamp(image[index + 1] + raiseAmount, 0.0f, 1.0f);
				image[index + 2] = std::clamp(image[index + 2] + raiseAmount, 0.0f, 1.0f);

				modified = true;
			}
		}
	}

	if (modified && heightMap != nullptr)
	{
		int updateWidth = endX - startX + 1;
		int updateHeight = endY - startY + 1;

		heightMap->updateSubImage(startX, startY, updateWidth, updateHeight, image, imageResolution);
	}
}

void BrushBrushModel::setHeightMap(Eng::Texture* texture)
{
	heightMap = texture;
}
