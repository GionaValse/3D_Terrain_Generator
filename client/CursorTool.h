#pragma once

#include "BaseTool.h"

class CursorTool : public BaseTool
{
public:
	CursorTool(float sensitivity);

	virtual void use(Eng::Camera* camera, float deltaX, float deltaY);

private:
	float mouseSensitivity = 0.1f;
};
