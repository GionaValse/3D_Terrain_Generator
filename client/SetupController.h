#pragma once
#include "SetupWindow.h"
#include "ConfigController.h"
#include "ISetupListener.h"

class SetupController : public ISetupListener
{
public:
	static SetupController& getInstance()
	{
		static SetupController instance;
		return instance;
	}

	void init()
	{
		if (this->setupWindow) return;

		ConfigController& config = ConfigController::getInstance();

		this->setupWindow = new SetupWindow(
			config.getActiveTextureConfig(),
			config.getActiveTerrainConfig()
		);

		this->setupWindow->setListener(this);
	}

	void free()
	{
	}

	void render(bool isGenerated) const
	{
		if (!isGenerated && this->setupWindow)
		{
			this->setupWindow->render();
		}
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

	TerrainConfig& getTerrainConfig() const
	{
		ConfigController& config = ConfigController::getInstance();
		return config.getActiveTerrainConfig();
	}

	SetupWindow* getWindow() const
	{
		return this->setupWindow;
	}

private:
	SetupController() : setupWindow(nullptr), generationRequested(false) {}
	SetupController(const SetupController&) = delete;
	SetupController& operator=(const SetupController&) = delete;

	SetupWindow* setupWindow;
	bool generationRequested;
};