#include "SmoothingBrushTool.h"

#include <algorithm>
#include <cmath>
#include <cstddef>

SmoothingBrushTool::SmoothingBrushTool(unsigned int radius, float strength, float falloff)
	: BaseBrushTool("Smoothing", "./res/smooth.png", radius, strength, falloff)
{}

SmoothingBrushTool& SmoothingBrushTool::getInstance()
{
	static SmoothingBrushTool instance;
	return instance;
}

void SmoothingBrushTool::applyBrushEffect(int x, int y, int pixelX, int pixelY, int pixelRadius, int resolution, std::vector<float>& image, bool& modified)
{
    float dx = static_cast<float>(x - pixelX);
    float dy = static_cast<float>(y - pixelY);
    float dist = std::sqrt(dx * dx + dy * dy);

    if (dist > static_cast<float>(pixelRadius)) {
        return;
    }

    float sum = 0.0f;
    int count = 0;
    for (int ny = -1; ny <= 1; ++ny) {
        for (int nx = -1; nx <= 1; ++nx) {
            int checkX = std::clamp(x + nx, 0, resolution - 1);
            int checkY = std::clamp(y + ny, 0, resolution - 1);
            sum += image[(checkY * resolution + checkX) * 3];
            count++;
        }
    }

    float average = sum / count;
    float influence = std::pow(1.0f - (dist / static_cast<float>(pixelRadius)), this->falloff);
    float actualStrength = strength * 4.0f;

    int index = (y * resolution + x) * 3;
    float currentVal = image[index];
    float smoothedVal = currentVal + (average - currentVal) * actualStrength * influence;

    if (std::abs(smoothedVal - currentVal) > 0.0001f) {
        modified = true;
    }

    image[index] = image[index + 1] = image[index + 2] = smoothedVal;
}

glm::vec3 SmoothingBrushTool::getRadiusColor() const
{
    return glm::vec3(0.490, 0.663, 0.780);
}
