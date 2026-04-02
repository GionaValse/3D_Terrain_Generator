#include "SolidVisualTool.h"

SolidVisualTool::SolidVisualTool()
    : BaseVisualTool("SolidVisualTool", "res/solid.png")
{}

SolidVisualTool& SolidVisualTool::getInstance()
{
	static SolidVisualTool instance;
	return instance;
}

void SolidVisualTool::use()
{
	Eng::Shader* shader = Eng::Shader::getCurrentInstance();

	if (shader)
	{
		int hasColorLoc = shader->getParamLocation("hasColor");
		shader->setBool(hasColorLoc, false);
	}

	Eng::Base::getInstance().changeWireFrame(false);
}
