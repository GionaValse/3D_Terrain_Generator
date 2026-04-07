#pragma once

#include <atomic>
#include <future>
#include <vector>

#include "EventDispatcher.h"
#include "StatusBar.h"

#include "engine.h"

using TopMenuDispatcher = EventDispatcher<>;

class AppController
{
public:
	static AppController& getInstance();

	~AppController() = default;

	void init(BaseWindow* topMenuBar = nullptr, StatusBar* statusBar = nullptr);
	void update();
	void free();

	void setTerrain(std::vector<float> imageArray, Eng::Mesh* terrainMesh);

	bool isExportingMesh() const;

private:
	AppController();

	AppController(const AppController&) = delete;
	AppController& operator=(const AppController&) = delete;

	BaseWindow* topMenuBar;
	StatusBar* statusBar;

	std::atomic<bool> isExporting{ false };
	bool wasExporting = false;

	std::thread exportThread;

	std::vector<float> image;
	Eng::Mesh* gridMesh;

	int onQuitSubscriptionId;
	int onExportMeshSubscriptionId;

	void onQuit();
	void onExportMesh();
};
