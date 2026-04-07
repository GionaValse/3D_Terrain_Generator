#include "WireframeVisualTool.h"

WireframeVisualTool::WireframeVisualTool()
    : BaseVisualTool("Wireframe", "res/wireframe.png")
{}

WireframeVisualTool& WireframeVisualTool::getInstance()
{
	static WireframeVisualTool instance;
	return instance;
}

void WireframeVisualTool::use()
{
	Eng::Shader* shader = Eng::Shader::getCurrentInstance();

	if (shader)
	{
		int hasColorLoc = shader->getParamLocation("hasColor");
		shader->setBool(hasColorLoc, false);
	}

	Eng::Base::getInstance().changeWireFrame(true);
}
