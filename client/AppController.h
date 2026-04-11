#pragma once

#include <atomic>
#include <future>
#include <thread>

#include "EventDispatcher.h"
#include "StatusBar.h"
#include "TerrainModel.h"

using TopMenuDispatcher = EventDispatcher<>;

class AppController
{
public:
	static AppController& getInstance();

	~AppController() = default;

	void init(StatusBar* statusBar = nullptr);
	void update();
	void free();

	void setTerrainModel(TerrainModel* terrain);

	bool isExportingMesh() const;

private:
	AppController();

	AppController(const AppController&) = delete;
	AppController& operator=(const AppController&) = delete;

	StatusBar* statusBar;

	std::atomic<float> exportProgress{ 0.0f };
	std::atomic<bool>  isExporting{ false };
	bool wasExporting = false;

	std::thread exportThread;

	TerrainModel* m_terrain;

	int onQuitSubscriptionId;
	int onExportMeshSubscriptionId;

	void onQuit();
	void onExportMesh();
};
