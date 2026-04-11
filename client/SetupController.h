#pragma once
#include "SetupWindow.h"
#include "TerrainConfig.h"
#include "TextureConfig.h"
#include "TerrainModel.h"
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
		this->setupWindow->setListener(this);
	}

	void free()
	{
		delete m_activeTerrain;
		m_activeTerrain = nullptr;
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

	TerrainConfig& getTerrainConfig() { return m_terrainConfig; }
	TextureConfig& getTextureConfig() { return m_textureConfig; }

	TerrainModel* getActiveTerrainModel() const { return m_activeTerrain; }
	void setActiveTerrainModel(TerrainModel* model)
	{
		delete m_activeTerrain;
		m_activeTerrain = model;
		if (this->setupWindow)
			this->setupWindow->setVisible(false);
	}

private:
	SetupController() : setupWindow(nullptr), m_activeTerrain(nullptr), generationRequested(false) {}
	SetupController(const SetupController&) = delete;
	SetupController& operator=(const SetupController&) = delete;

	SetupWindow*  setupWindow;
	TerrainConfig m_terrainConfig;
	TextureConfig m_textureConfig;
	TerrainModel* m_activeTerrain;
	bool generationRequested;
};
