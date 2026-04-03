#include "PointerController.h"

#include "ConfigController.h"
#include "CameraGestureController.h"

#include "CursorTool.h"
#include "BaseBrushTool.h"

PointerController::PointerController()
	: ToolController(),
	mouseMoveSubscriptionId(-1)
{}

PointerController& PointerController::getInstance()
{
	static PointerController instance;
	return instance;
}

PointerController::~PointerController()
{}

void PointerController::init(ToolWindow* window)
{
	this->toolWindow = window;

	if (this->toolWindow)
	{
		this->toolWindow->setListener(this);
	}

	mouseMoveSubscriptionId = MouseMoveDispatcher::getInstance().subscribe(
		"LEFT_MOUSE_MOVE", 
		[this](int x, int y, int lastX, int lastY)
		{
			this->onCursorMove(x, y, lastX, lastY);
		}
	);
}

void PointerController::free() const
{
	MouseMoveDispatcher::getInstance().unsubscribe("LEFT_MOUSE_MOVE", mouseMoveSubscriptionId);
}

void PointerController::setHeightMap(Eng::Texture* texture)
{
	this->heightMapTexture = texture;
}

BaseTool* PointerController::getActiveTool() const
{
	return this->currentTool;
}

void PointerController::onToolSelected(BaseTool* tool)
{
	this->currentTool = tool;
}

void PointerController::onCursorMove(int x, int y, int lastX, int lastY)
{
	auto* activeTool = getActiveTool();
	if (!activeTool) return;

	if (auto* brushTool = dynamic_cast<BaseBrushTool*>(activeTool))
	{
		glm::vec3 clickedPos;
		if (Eng::Base::getInstance().getClickedNode(x, y, clickedPos))
		{
			ConfigController& config = ConfigController::getInstance();
			std::vector<float>& imageData = config.getHeightMapImage();
			int resolution = config.getActiveTerrainConfig().size;

			UpdateArea area = brushTool->use(clickedPos, config.getActiveTerrainConfig(), config.getActiveTextureConfig(), imageData);

			if (area.isModified && this->heightMapTexture != nullptr)
			{
				this->heightMapTexture->updateSubImage(
					area.startX,
					area.startY,
					area.width,
					area.height,
					imageData,
					resolution
				);
			}
		}
	}
	else if (auto* cursorTool = dynamic_cast<CursorTool*>(activeTool))
	{
		CameraGestureController::getInstance().cameraRotate(x, y, lastX, lastY);
	}
}
