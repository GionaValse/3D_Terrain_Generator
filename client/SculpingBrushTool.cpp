#include "SculpingBrushTool.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

SculptingBrushTool::SculptingBrushTool(unsigned int radius, float strength, float falloff)
	: BaseBrushTool("Sculpting", "./res/paint.png", radius, strength, falloff)
{}

SculptingBrushTool& SculptingBrushTool::getInstance()
{
	static SculptingBrushTool instance;
	return instance;
}

void SculptingBrushTool::applyBrushEffect(
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

		image[index + 0] = std::clamp(image[index + 0] + raiseAmount, 0.0f, 1.0f);
		image[index + 1] = std::clamp(image[index + 1] + raiseAmount, 0.0f, 1.0f);
		image[index + 2] = std::clamp(image[index + 2] + raiseAmount, 0.0f, 1.0f);

		modified = true;
	}
}

glm::vec3 SculptingBrushTool::getRadiusColor() const
{
	return glm::vec3(0.435, 0.561, 0.416);
}
