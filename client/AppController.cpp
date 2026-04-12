#include "AppController.h"

#include "AppEvents.h"
#include "ObjExporter.hpp"
#include "SetupController.h"

#include "engine.h"

AppController::AppController()
    : statusBar(nullptr),
    isTaskRunning(false),
    currentOnCompleteCallback(nullptr),
    onQuitSubscriptionId(-1),
    onExportMeshSubscriptionId(-1)
{}

AppController& AppController::getInstance()
{
    static AppController instance;
    return instance;
}

void AppController::init(StatusBar* statusBar)
{
    this->statusBar = statusBar;

    onQuitSubscriptionId = TopMenuDispatcher::getInstance().subscribe(AppEvents::APP_EXIT, [this]() { this->onQuit(); });
    onExportMeshSubscriptionId = TopMenuDispatcher::getInstance().subscribe(AppEvents::MENU_EXPORT_MESH, [this]() { this->onExportMesh(); });
}

void AppController::update()
{
    if (!statusBar) return;

    if (isTaskRunning.load())
    {
        statusBar->setProgress(true, currentTaskProgress.load());
    }
    else if (wasTaskRunning)
    {
        statusBar->setMessage("Ready");
        statusBar->setProgress(false);

        wasTaskRunning = false;

        if (currentOnCompleteCallback)
        {
            currentOnCompleteCallback();
            currentOnCompleteCallback = nullptr;
        }
    }
}

void AppController::free()
{
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::APP_EXIT, onQuitSubscriptionId);
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::MENU_EXPORT_MESH, onExportMeshSubscriptionId);

    if (const_cast<AppController*>(this)->workerThread.joinable())
    {
        const_cast<AppController*>(this)->workerThread.join();
    }
}

void AppController::runBackgroundTask(const std::string& message, std::function<void(std::atomic<float>*)> task, std::function<void()> onComplete)
{
    if (isTaskRunning.load())
    {
        if (statusBar)
            statusBar->setMessage("Wait until task finish!");
        return;
    }

    wasTaskRunning = true;
    isTaskRunning.store(true);
    currentTaskProgress.store(0.0f);
    currentOnCompleteCallback = onComplete;

    if (statusBar)
    {
        statusBar->setMessage(message);
        statusBar->setProgress(true, 0.0f);
    }

    if (workerThread.joinable())
        workerThread.join();

    workerThread = std::thread([this, task]() 
        {
            task(&currentTaskProgress);
            isTaskRunning.store(false);
        }
    );
}

bool AppController::isBackgroundTaskRunning() const
{
    return isTaskRunning;
}

void AppController::onQuit()
{
    Eng::Base::getInstance().stop();
}

void AppController::onExportMesh()
{
    TerrainModel* terrain = SetupController::getInstance().getActiveTerrainModel();
    if (!terrain) return;

    TerrainConfig terrainConfig = terrain->getTerrainConfig();

    runBackgroundTask(
        "Exporting...",
        [
            imgData = terrain->getTerrainImage(),
            size = terrainConfig.size,
            hScale = terrainConfig.heightScale,
            &mesh = *terrain->getTerrainMesh(),
            this
        ](std::atomic<float>* progress)
        {
            ObjExporter::exportToObj("./bin/export/terrain.obj", mesh, imgData, size, hScale, progress);
        }
    );
}
