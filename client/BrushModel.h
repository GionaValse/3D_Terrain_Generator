#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

#include "engine.h"

#include "TerrainConfig.hpp"

enum class BrushToolType { NONE, BRUSH, EROSION, FLATTEN, SMOOTH };

class BrushModel
{
public:
	BrushModel(std::string name = "Brush", std::string res = "", BrushToolType toolType = BrushToolType::NONE);

	~BrushModel();
	
	virtual void use(glm::vec3 coords, terrain::TerrainConfig config, std::vector<float>& image);

	std::string getName() const;
	int getIconId() const;
	BrushToolType getToolType() const;

private:
	std::string name;
	int iconId;
	BrushToolType toolType;

	Eng::Texture* iconTexture;
};
