#pragma once
#include "SetupWindow.h"
#include "ConfigModel.h"
#include "ISetupListener.h"

class SetupController : public ISetupListener
{
public:
	static SetupController& getInstance()
	{
		static SetupController instance;
		return instance;
	}

	void init(ConfigModel& config)
	{
		if (this->setupWindow) return;

		m_config = &config;

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
		return m_config->getActiveTerrainConfig();
	}

	SetupWindow* getWindow() const
	{
		return this->setupWindow;
	}

private:
	SetupController() : setupWindow(nullptr), m_config(nullptr), generationRequested(false) {}
	SetupController(const SetupController&) = delete;
	SetupController& operator=(const SetupController&) = delete;

	SetupWindow* setupWindow;
	ConfigModel* m_config;
	bool generationRequested;
};
