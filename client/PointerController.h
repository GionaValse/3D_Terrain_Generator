#pragma once

#include "ToolController.h"

#include "ToolWindow.h"
#include "EventDispatcher.h"
#include "TerrainModel.h"

using MouseMoveDispatcher = EventDispatcher<int, int, int, int>;

class PointerController : public ToolController
{
public:
	static PointerController& getInstance();

	~PointerController();

	void init(ToolWindow* window, IToolSettingsWindow* editorWindow = nullptr) override;
	void setTerrainModel(TerrainModel* terrain);
	void free() const override;

	BaseTool* getActiveTool() const override;

	void onToolSelected(BaseTool* tool, int groupPos, int itemPos) override;
	void onToolEditor(BaseTool* tool, int groupPos, int itemPos) override;

private:
	PointerController();

	PointerController(const PointerController&) = delete;
	void operator=(const PointerController&) = delete;

	TerrainModel* m_terrain;

	size_t mouseMoveSubscriptionId;
	size_t mouseHoverSubscriptionId;

	int brushPositionLoc;
	int brushRadiusLoc;
	int brushActiveLoc;
	int brushRadiusColorLoc;

	void onCursorMove(int x, int y, int lastX, int lastY);
	void onCursorHover(int x, int y);

	void hideBrushArea();
	void showBrushArea(BaseBrushTool* brush, glm::vec3 mousePos);
};
