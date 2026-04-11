#include "PointerController.h"

#include "AppEvents.h"

#include "CameraGestureController.h"

#include "CursorTool.h"
#include "BaseBrushTool.h"

PointerController::PointerController()
	: ToolController(),
	m_terrain(nullptr),
	mouseMoveSubscriptionId(-1),
	mouseHoverSubscriptionId(-1),
	brushPositionLoc(-1),
	brushRadiusLoc(-1),
	brushActiveLoc(-1),
	brushRadiusColorLoc(-1)
{}

PointerController& PointerController::getInstance()
{
	static PointerController instance;
	return instance;
}

PointerController::~PointerController()
{}

void PointerController::setTerrainModel(TerrainModel* terrain)
{
	this->m_terrain = terrain;
}

void PointerController::init(ToolWindow* window, IToolSettingsWindow* editorWindow)
{
	this->toolWindow = window;
	this->editorToolWindow = editorWindow;

	if (this->toolWindow)
	{
		this->toolWindow->setListener(this);
	}

	mouseMoveSubscriptionId = MouseMoveDispatcher::getInstance().subscribe(
		AppEvents::LEFT_MOUSE_MOVE,
		[this](int x, int y, int lastX, int lastY)
		{
			this->onCursorMove(x, y, lastX, lastY);
		}
	);

	mouseHoverSubscriptionId = MouseMoveDispatcher::getInstance().subscribe(
		AppEvents::MOUSE_HOVER,
		[this](int x, int y, int lastX, int lastY)
		{
			this->onCursorHover(x, y);
		}
	);
}

void PointerController::free() const
{
	MouseMoveDispatcher::getInstance().unsubscribe(AppEvents::LEFT_MOUSE_MOVE, mouseMoveSubscriptionId);
}

BaseTool* PointerController::getActiveTool() const
{
	return this->currentTool;
}

void PointerController::onToolSelected(BaseTool* tool, int groupPos, int itemPos)
{
	this->currentTool = tool;

	Eng::Shader* shader = Eng::Shader::getCurrentInstance();

	if (shader)
	{
		brushPositionLoc = shader->getParamLocation("brushPosition");
		brushRadiusLoc = shader->getParamLocation("brushRadius");
		brushActiveLoc = shader->getParamLocation("isBrushActive");
		brushRadiusColorLoc = shader->getParamLocation("brushRadiusColor");
	}

	if (this->editorToolWindow)
	{
		auto* brushTool = dynamic_cast<BaseBrushTool*>(tool);
		if (brushTool)
			editorToolWindow->setActiveTool(brushTool);
		else
			editorToolWindow->setVisible(false);
	}
}

void PointerController::onToolEditor(BaseTool* tool, int groupPos, int itemPos)
{
	auto* brushTool = dynamic_cast<BaseBrushTool*>(tool);
	if (!brushTool) return;

	editorToolWindow->setActiveTool(brushTool);
	editorToolWindow->setVisible(true);
	this->currentTool = brushTool;
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
			showBrushArea(brushTool, clickedPos);

			std::vector<float>& imageData = m_terrain->getTerrainImage();
			int resolution = m_terrain->getTextureConfig().size;

			UpdateArea area = brushTool->use(clickedPos, m_terrain->getTerrainConfig(), m_terrain->getTextureConfig(), imageData);

			if (area.isModified && m_terrain->getHeightMapTexture())
			{
				m_terrain->getHeightMapTexture()->updateSubImage(
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
		hideBrushArea();
	}
}

void PointerController::onCursorHover(int x, int y)
{
	auto* brushTool = dynamic_cast<BaseBrushTool*>(currentTool);
	glm::vec3 hoverPos;

	if (brushTool && Eng::Base::getInstance().getClickedNode(x, y, hoverPos))
		showBrushArea(brushTool, hoverPos);
	else
		hideBrushArea();
}

void PointerController::hideBrushArea()
{
	Eng::Shader* shader = Eng::Shader::getCurrentInstance();
	if (shader) shader->setBool(brushActiveLoc, false);
}

void PointerController::showBrushArea(BaseBrushTool* brush, glm::vec3 mousePos)
{
	Eng::Shader* shader = Eng::Shader::getCurrentInstance();
	if (!shader) return;

	shader->setBool(brushActiveLoc, true);
	shader->setFloat(brushRadiusLoc, brush->getRadius());
	shader->setVec3(brushRadiusColorLoc, brush->getRadiusColor());

	Eng::Camera* camera = Eng::Base::getInstance().getActiveCamera();
	glm::vec4 eyeBrushPos = camera->getViewMatrix() * glm::vec4(mousePos, 1.0f);
	shader->setVec4(brushPositionLoc, eyeBrushPos);
}
