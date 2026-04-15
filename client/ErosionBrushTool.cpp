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

void ErosionBrushTool::applyBrushEffect(
	int x, int y,
	int pixelX, int pixelY,
	int pixelRadius,
	int resolution,
	float heightScale,
	float deltaTime,
	std::vector<float>& image,
	bool& modified
)
{
	float distX = (float)(x - pixelX);
	float distY = (float)(y - pixelY);
	float distance = std::sqrt(distX * distX + distY * distY);

	if (distance <= pixelRadius)
	{
		float t = distance / pixelRadius;

		float smooth = 1.0f - (t * t * (3.0f - 2.0f * t));
		float influence = (1.0f - this->falloff) * 1.0f + (this->falloff * smooth);
		float pixelStrength = this->strength / heightScale;

		float raiseAmount = pixelStrength * influence * deltaTime;
		int index = (y * resolution + x) * 3;

		image[index + 0] = std::clamp(image[index + 0] - raiseAmount, 0.0f, 1.0f);
		image[index + 1] = std::clamp(image[index + 1] - raiseAmount, 0.0f, 1.0f);
		image[index + 2] = std::clamp(image[index + 2] - raiseAmount, 0.0f, 1.0f);

		modified = true;
	}
}

glm::vec3 ErosionBrushTool::getRadiusColor() const
{
	return glm::vec3(0.831, 0.416, 0.416);
}
