#include "ErosionBrushTool.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

ErosionBrushTool::ErosionBrushTool(unsigned int radius, float strength, float falloff)
	: BaseBrushTool("Erosion", "./res/eraser.png", radius, strength, falloff)
{}

ErosionBrushTool& ErosionBrushTool::getInstance()
{
	static ErosionBrushTool instance;
	return instance;
}

void ErosionBrushTool::applyBrushEffect(int x, int y, int pixelX, int pixelY, int resolution, std::vector<float>& image, bool& modified)
{
	float distX = (float)(x - pixelX);
	float distY = (float)(y - pixelY);
	float distance = std::sqrt(distX * distX + distY * distY);

	if (distance <= radius)
	{
		float falloff = 1.0f - (distance / radius);
		float raiseAmount = -(strength * falloff);
		int index = (y * resolution + x) * 3;

		image[index + 0] = std::clamp(image[index + 0] + raiseAmount, 0.0f, 1.0f);
		image[index + 1] = std::clamp(image[index + 1] + raiseAmount, 0.0f, 1.0f);
		image[index + 2] = std::clamp(image[index + 2] + raiseAmount, 0.0f, 1.0f);

		modified = true;
	}
}
