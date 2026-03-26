#pragma once

#include "BrushModel.h"

class BrushBrushModel : public BrushModel
{
public:
	BrushBrushModel(std::string name = "Brush", std::string res = "");

	~BrushBrushModel();

	void use(glm::vec3 coords, terrain::TerrainConfig config, std::vector<float>& image) override;

	void setHeightMap(Eng::Texture* texture);

private:
	Eng::Texture* heightMap;
};
