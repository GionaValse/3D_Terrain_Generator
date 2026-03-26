#pragma once

#include "BrushModel.h"

class ErosionBrushModel : public BrushModel
{
public:
	ErosionBrushModel(std::string name = "Brush", std::string res = "");

	~ErosionBrushModel();

	void use(glm::vec3 coords, terrain::TerrainConfig config, std::vector<float>& image) override;

	void setHeightMap(Eng::Texture* texture);

private:
	Eng::Texture* heightMap;
};
