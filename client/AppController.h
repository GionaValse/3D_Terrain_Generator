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

	void runBackgroundTask(
		const std::string& message, 
		std::function<void(std::atomic<float>*)> task,
		std::function<void()> onComplete = nullptr
	);

	bool isBackgroundTaskRunning() const;

private:
	AppController();

	AppController(const AppController&) = delete;
	AppController& operator=(const AppController&) = delete;

	StatusBar* statusBar;

	std::atomic<float> currentTaskProgress{ 0.0f };
	std::atomic<bool>  isTaskRunning{ false };
	std::function<void()> currentOnCompleteCallback;
	bool wasTaskRunning = false;

	std::thread workerThread;

	int onQuitSubscriptionId;
	int onExportMeshSubscriptionId;

	void onQuit();
	void onExportMesh();
};
