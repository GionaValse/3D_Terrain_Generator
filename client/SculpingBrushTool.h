#pragma once

#include "BaseBrushTool.h"

class SculptingBrushTool : public BaseBrushTool
{
public:
	static SculptingBrushTool& getInstance();

	void applyBrushEffect(int x, int y, int pixelX, int pixelY, int pixelRadius, int resolution, std::vector<float>& image, bool& modified) override;

private:
	SculptingBrushTool(unsigned int radius = 15, float strength = 0.005f, float falloff = 1.0f);

	SculptingBrushTool(const SculptingBrushTool&) = delete;
	void operator=(const SculptingBrushTool&) = delete;
};
