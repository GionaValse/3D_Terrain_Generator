#include "PointerController.h"

#include "ConfigController.h"

#include "CursorTool.h"
#include "BaseBrushTool.h"

#include "ErosionBrushTool.h"
#include "SculpingBrushTool.h"
#include "SmoothingBrushTool.h"

PointerController::PointerController()
	: ToolController(),
	toolWindow(nullptr),
	mouseMoveSubscriptionId(-1)
{}

PointerController& PointerController::getInstance()
{
	static PointerController instance;
	return instance;
}

PointerController::~PointerController()
{
	MouseMoveDispatcher::getInstance().unsubscribe("LEFT_MOUSE_MOVE", mouseMoveSubscriptionId);
}

void PointerController::init(ToolWindow* window)
{
	toolWindow = window;

	mouseMoveSubscriptionId = MouseMoveDispatcher::getInstance().subscribe(
		"LEFT_MOUSE_MOVE", 
		[this](int x, int y, int lastX, int lastY)
		{
			this->onCursorMove(x, y, lastX, lastY);
		}
	);
}

void PointerController::setActiveTool(BaseTool* tool)
{
	if (toolWindow)
	{
		toolWindow->setCurrentTool(tool);
	}
}

BaseTool* PointerController::getActiveTool() const
{
	if (toolWindow)
		return toolWindow->getCurrentTool();
	return nullptr;
}

void PointerController::setHeightMapForTools(Eng::Texture* texture)
{
	ErosionBrushTool::getInstance().setHeightMap(texture);
	SculptingBrushTool::getInstance().setHeightMap(texture);
	SmoothingBrushTool::getInstance().setHeightMap(texture);
}

void PointerController::onCursorMove(int x, int y, int lastX, int lastY)
{
	auto* activeTool = getActiveTool();
	if (!activeTool) return;

	float deltaX = (float)(x - lastX);
	float deltaY = (float)(y - lastY);

	if (auto* brushTool = dynamic_cast<BaseBrushTool*>(activeTool))
	{
		glm::vec3 clickedPos;
		if (Eng::Base::getInstance().getClickedNode(x, y, clickedPos))
		{
			ConfigController& config = ConfigController::getInstance();
			brushTool->use(clickedPos, config.getActiveTerrainConfig(), config.getHeightMapImage());
		}
	}
	else if (auto* cursorTool = dynamic_cast<CursorTool*>(activeTool))
	{
		Eng::Camera* camera = Eng::Base::getInstance().getActiveCamera();
		cursorTool->use(camera, deltaX, deltaY);
	}
}
