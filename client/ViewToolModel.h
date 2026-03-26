#pragma once

#include "engine.h"

enum class ViewToolType { SHADED, SOLID, WIREFRAME };

class ViewToolModel
{
public:
	ViewToolModel(std::string name = "Brush", std::string res = "", ViewToolType toolType = ViewToolType::SOLID);

	~ViewToolModel();

	virtual void use();

	std::string getName() const;
	int getIconId() const;
	ViewToolType getToolType() const;

private:
	std::string name;
	int iconId;
	ViewToolType toolType;

	Eng::Texture* iconTexture;
};
