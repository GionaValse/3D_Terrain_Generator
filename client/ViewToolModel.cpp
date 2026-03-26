#include "ViewToolModel.h"

ViewToolModel::ViewToolModel(std::string name, std::string res, ViewToolType toolType)
	: name(name),
	iconId(-1),
	toolType(toolType)
{
	iconTexture = new Eng::Texture(name + "Texture", res);
	iconId = iconTexture->getTexId();
}

ViewToolModel::~ViewToolModel()
{
	if (iconTexture)
		delete iconTexture;
}

void ViewToolModel::use()
{}

std::string ViewToolModel::getName() const
{
	return name;
}

int ViewToolModel::getIconId() const
{
	return iconId;
}

ViewToolType ViewToolModel::getToolType() const
{
	return toolType;
}
