#pragma once
#include "SetupWindow.h"

class SetupController
{
public:
	static SetupController& getInstance()
	{
		static SetupController instance;
		return instance;
	}

	void init(SetupWindow* window)
	{
		m_setupWindow = window;
	}

	void render(bool isGenerated)
	{
		if (!isGenerated && m_setupWindow)
		{
			m_setupWindow->render();
		}
	}

	bool shouldGenerateTerrain()
	{
		if (m_setupWindow)
		{
			return m_setupWindow->checkAndResetTrigger();
		}
		return false;
	}

	float getTerrainHeightScale() const
	{
		if (m_setupWindow)
		{
			return m_setupWindow->getHeightScale();
		}
		return 100.0f;
	}

private:
	SetupController() = default;
	SetupController(const SetupController&) = delete;
	SetupController& operator=(const SetupController&) = delete;

	SetupWindow* m_setupWindow = nullptr;
};