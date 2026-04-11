#pragma once

#include <atomic>
#include <future>
#include <thread>

#include "EventDispatcher.h"
#include "StatusBar.h"

using TopMenuDispatcher = EventDispatcher<>;

class AppController
{
public:
	static AppController& getInstance();

	~AppController() = default;

	void init(StatusBar* statusBar = nullptr);
	void update();
	void free();

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

	int onQuitSubscriptionId;
	int onExportMeshSubscriptionId;

	void onQuit();
	void onExportMesh();
};
