#include "AppController.h"

#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <shellapi.h>
#endif

#include "AppEvents.h"
#include "SetupController.h"

#include "ImageExporter.hpp"
#include "ObjExporter.hpp"

#include "engine.h"

AppController::AppController()
    : statusBar(nullptr),
    isTaskRunning(false),
    currentOnCompleteCallback(nullptr),
    onQuitSubscriptionId(-1),
    onOpenDocSubscriptionId(-1),
    onExportMeshSubscriptionId(-1),
    onExportImageSubscriptionId(-1)
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
    onOpenDocSubscriptionId = TopMenuDispatcher::getInstance().subscribe(AppEvents::APP_DOCUMENTATION, [this]() { this->onOpenDocumentation(); });

    onExportMeshSubscriptionId = TopMenuDispatcher::getInstance().subscribe(AppEvents::MENU_EXPORT_MESH, [this]() { this->onExportMesh(); });
    onExportImageSubscriptionId = TopMenuDispatcher::getInstance().subscribe(AppEvents::MENU_EXPORT_HEIGHTMAP, [this]() { this->onExportImage(); });
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
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::APP_DOCUMENTATION, onOpenDocSubscriptionId);
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::MENU_EXPORT_MESH, onExportMeshSubscriptionId);
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::MENU_EXPORT_HEIGHTMAP, onExportImageSubscriptionId);

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

void AppController::onOpenDocumentation()
{
#if defined(_WIN32)
    ShellExecute(0, L"open", L"https://github.com/GionaValse/3D_Terrain_Generator", 0, 0, SW_SHOWNORMAL);
#endif
}

void AppController::onExportMesh()
{
    TerrainModel* terrain = SetupController::getInstance().getActiveTerrainModel();
    if (!terrain) return;

    TerrainConfig terrainConfig = terrain->getTerrainConfig();
    std::vector<Eng::Mesh*> chunks = terrain->getTerrainChunks();
    if (chunks.empty()) return;

    runBackgroundTask(
        "Terrain export...",
        [
            imgData = terrain->getTerrainImage(),
            size = terrainConfig.size,
            hScale = terrainConfig.heightScale,
            chunks,
            this
        ](std::atomic<float>* progress)
        {
            ObjExporter::exportToObj("./bin/export/terrain.obj", chunks, imgData, size, hScale, 0, progress);
        }
    );
}

void AppController::onExportImage()
{
    TerrainModel* model = SetupController::getInstance().getActiveTerrainModel();

    runBackgroundTask(
        "HeightMap export...", 
        [
            heightmap = model->getTerrainImage(),
            textureConfig = model->getTextureConfig(),
            this
        ](std::atomic<float>* progress) 
        {
            std::string textName;
            if (!terrain::ImageExporter::saveEXR(heightmap, textureConfig, textName))
            {
                std::cerr << "Unable to save the texture image" << std::endl;
                return;
            }
        }
    );
}
