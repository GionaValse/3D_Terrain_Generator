#pragma once
#include "SetupWindow.h"
#include "ISetupListener.h"

class SetupController : public ISetupListener
{
public:
	static SetupController& getInstance()
	{
		static SetupController instance;
		return instance;
	}

	void init(SetupWindow* window)
	{
		this->setupWindow = window;

		if (this->setupWindow)
		{
			this->setupWindow->setListener(this); 
		}
	}

	void render(bool isGenerated) const
	{
		if (!isGenerated && this->setupWindow)
		{
			this->setupWindow->render();
		}
	}

	void free() const
	{
	
	}

	void onTerrainGenerationRequested() override
	{

		this->generationRequested = true;
	}

	bool consumeGenerationRequest()
	{
		bool val = this->generationRequested;
		this->generationRequested = false;
		return val;
	}

	float getTerrainHeightScale() const
	{
		return this->setupWindow ? this->setupWindow->getHeightScale() : 100.0f;
	}

private:
	SetupController() : setupWindow(nullptr), generationRequested(false) {}
	SetupController(const SetupController&) = delete;
	SetupController& operator=(const SetupController&) = delete;

	SetupWindow* setupWindow;
	bool generationRequested;
};