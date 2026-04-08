#pragma once

#include "BaseBrushTool.h"

class ErosionBrushTool : public BaseBrushTool
{
public:
	static ErosionBrushTool& getInstance();

	void applyBrushEffect(int x, int y, int pixelX, int pixelY, int pixelRadius, int resolution, std::vector<float>& image, bool& modified) override;

	glm::vec3 getRadiusColor() const override;

private:
	ErosionBrushTool(unsigned int radius = 15, float strength = 0.005f, float falloff = 1.0f);

	ErosionBrushTool(const ErosionBrushTool&) = delete;
	void operator=(const ErosionBrushTool&) = delete;
};
