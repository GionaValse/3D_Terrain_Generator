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

void SmoothingBrushTool::applyBrushEffect(int x, int y, int pixelX, int pixelY, int resolution, std::vector<float>& image, bool& modified)
{
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

	float dist = 1.0f; // Calculate distance from the center of the brush
    float influence = std::pow(1.0f - (dist / radius), this->falloff);

    int index = (y * resolution + x) * 3;
    float currentVal = image[index];

    float smoothedVal = currentVal + (average - currentVal) * strength * influence;

    image[index] = image[index + 1] = image[index + 2] = smoothedVal;
}
