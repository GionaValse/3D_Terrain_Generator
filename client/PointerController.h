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

	void init(ToolWindow* window);

	void setActiveTool(BaseTool* tool) override;

	BaseTool* getActiveTool() const override;

	void setHeightMapForTools(Eng::Texture* texture);

private:
	PointerController();

	PointerController(const PointerController&) = delete;
	void operator=(const PointerController&) = delete;

	size_t mouseMoveSubscriptionId;

	void onCursorMove(int x, int y, int lastX, int lastY);

protected:
	ToolWindow* toolWindow;
};
