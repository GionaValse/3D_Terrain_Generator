#pragma once

#include "ToolController.h"

#include "ToolWindow.h"
#include "EventDispatcher.h"

using MouseMoveDispatcher = EventDispatcher<int, int, int, int>;

class PointerController : public ToolController
{
public:
	static PointerController& getInstance();

	~PointerController();

	void init(ToolWindow* window, IToolSettingsWindow* editorWindow) override;
	void free() const override;

	void setHeightMap(Eng::Texture* texture);

	BaseTool* getActiveTool() const override;

	void onToolSelected(BaseTool* tool, int groupPos, int itemPos) override;
	void onToolEditor(BaseTool* tool, int groupPos, int itemPos) override;

private:
	PointerController();

	PointerController(const PointerController&) = delete;
	void operator=(const PointerController&) = delete;

	size_t mouseMoveSubscriptionId;

	Eng::Texture* heightMapTexture;

	void onCursorMove(int x, int y, int lastX, int lastY);
};
