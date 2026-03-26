#include "BrushModel.h"

BrushModel::BrushModel(std::string name, std::string res, BrushToolType toolType)
	: name(name),
	iconId(-1),
	toolType(toolType)
{
	iconTexture = new Eng::Texture(name + "Texture", res);
	iconId = iconTexture->getTexId();
}

BrushModel::~BrushModel()
{
	if (iconTexture)
		delete iconTexture;
}

void BrushModel::use(glm::vec3 coords, terrain::TerrainConfig config, std::vector<float>&image)
{}

std::string BrushModel::getName() const
{
	return name;
}

int BrushModel::getIconId() const
{
	return iconId;
}

BrushToolType BrushModel::getToolType() const
{
	return toolType;
}
