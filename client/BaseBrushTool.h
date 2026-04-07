#pragma once

#include "BaseTool.h"
#include "TerrainConfig.h"
#include "TextureConfig.h"

struct UpdateArea
{
	int startX = 0;
	int startY = 0;
	int width = 0;
	int height = 0;
	bool isModified = false;
};

class BaseBrushTool : public BaseTool
{
public:
	BaseBrushTool(std::string name = "BrushTool", std::string res = "", unsigned int radius = 15, float strenght = 0.005f, float falloff = 0.5f) 
		: BaseTool(name, res),
		radius(radius), 
		strength(strenght),
		falloff(falloff)
	{}

	virtual void applyBrushEffect(int x, int y, int pixelX, int pixelY, int pixelRadius, int resolution, std::vector<float>& image, bool& modified) = 0;

	virtual UpdateArea use(glm::vec3 coords, TerrainConfig terConfig, TextureConfig texConfig, std::vector<float>& image)
	{
		float terrainPhysicalSize = terConfig.size;
		int imageResolution = texConfig.size;

		float localX = coords.x + (terrainPhysicalSize / 2.0f);
		float localZ = coords.z + (terrainPhysicalSize / 2.0f);

		float u = localX / terrainPhysicalSize;
		float v = localZ / terrainPhysicalSize;

		int pixelX = static_cast<int>(u * imageResolution);
		int pixelY = static_cast<int>(v * imageResolution);

		if (pixelX < 0 || pixelX >= imageResolution || pixelY < 0 || pixelY >= imageResolution)
		{
			UpdateArea area;
			area.isModified = false;
			return area;
		}

		int startX = std::max(0, pixelX - (int)radius);
		int startY = std::max(0, pixelY - (int)radius);
		int endX = std::min(imageResolution - 1, pixelX + (int)radius);
		int endY = std::min(imageResolution - 1, pixelY + (int)radius);

		bool modified = false;

		for (int y = startY; y <= endY; ++y)
		{
			for (int x = startX; x <= endX; ++x)
			{
				applyBrushEffect(x, y, pixelX, pixelY, (int)radius, imageResolution, image, modified);
			}
		}

		UpdateArea area;
		area.isModified = modified;

		if (modified)
		{
			area.startX = startX;
			area.startY = startY;
			area.width = endX - startX + 1;
			area.height = endY - startY + 1;
		}

		return area;
	}

	unsigned int getRadius() const { return radius; }
	float getStrength() const { return strength; }
	float getFalloff() const { return falloff; }

	void setRadius(unsigned int r) { radius = r; }
	void setStrength(float s) { strength = s; }
	void setFalloff(float f) { falloff = f; }

protected:
	unsigned int radius;	// How large is the affected area.
	float strength;			// How quickly the ground rises or falls.
	float falloff;			// How soft the edge of the brush is (whether the effect is sharp in the center and fades out at the edges).
};
