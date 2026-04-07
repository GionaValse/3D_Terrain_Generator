#include "ShadesVisualTool.h"

ShadesVisualTool::ShadesVisualTool()
	: BaseVisualTool("Shades", "res/shades.png")
{}

ShadesVisualTool& ShadesVisualTool::getInstance()
{
	static ShadesVisualTool instance;
	return instance;
}

void ShadesVisualTool::use()
{
	Eng::Shader* shader = Eng::Shader::getCurrentInstance();

	if (shader)
	{
		int hasColorLoc = shader->getParamLocation("hasColor");
		shader->setBool(hasColorLoc, true);
	}

	Eng::Base::getInstance().changeWireFrame(false);
}
