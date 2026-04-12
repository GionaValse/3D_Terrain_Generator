#include "SetupController.h"

#include "AppEvents.h"

SetupController::SetupController()
	: setupWindow(nullptr),
	m_activeTerrain(nullptr),
	isGenerated(false),
	newGenerationSubscriptionId(-1)
{}

SetupController& SetupController::getInstance()
{
	static SetupController instance;
	return instance;
}

void SetupController::init(SetupWindow* window)
{
	this->setupWindow = window;
	this->setupWindow->setListener(this);

	newGenerationSubscriptionId = MenuDispatcer::getInstance().subscribe(
		AppEvents::MENU_NEW_PROJECT,
		[this]()
		{
			onNewGeneration();
		}
	);

	onNewGeneration();
}

void SetupController::free()
{
	delete m_activeTerrain;
	m_activeTerrain = nullptr;

	MenuDispatcer::getInstance().unsubscribe(AppEvents::MENU_NEW_PROJECT, newGenerationSubscriptionId);
}

void SetupController::onTerrainGenerationRequest(TerrainConfig terrainConfig, TextureConfig textureConfig)
{
	if (m_activeTerrain)
	{
		delete m_activeTerrain;
		m_activeTerrain = nullptr;
	}

	m_activeTerrain = new TerrainModel(terrainConfig, textureConfig);

	if (setupWindow)
		setupWindow->setVisible(false);

	isGenerated = true;
}

TerrainModel* SetupController::getActiveTerrainModel() const
{
	return m_activeTerrain;
}

bool SetupController::isTerrainGenerated() const
{
	return isGenerated;
}

void SetupController::onNewGeneration()
{
	if (setupWindow)
		setupWindow->setVisible(true);

	isGenerated = false;
}
