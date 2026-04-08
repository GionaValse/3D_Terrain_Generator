#pragma once

#include "BaseBrushTool.h"

class SmoothingBrushTool : public BaseBrushTool
{
public:
	static SmoothingBrushTool& getInstance();

	void applyBrushEffect(int x, int y, int pixelX, int pixelY, int pixelRadius, int resolution, std::vector<float>& image, bool& modified) override;

	glm::vec3 getRadiusColor() const override;

private:
	SmoothingBrushTool(unsigned int radius = 15, float strength = 0.05f, float falloff = 1.0f);

	SmoothingBrushTool(const SmoothingBrushTool&) = delete;
	void operator=(const SmoothingBrushTool&) = delete;
};
