#include "AppController.h"

#include "AppEvents.h"
#include "ObjExporter.hpp"
#include "SetupController.h"

#include "engine.h"

AppController::AppController()
    : statusBar(nullptr),
    isExporting(false),
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

    if (isExporting.load())
    {
        statusBar->setProgress(true, exportProgress.load());
    }
    else if (wasExporting)
    {
        statusBar->setMessage("Ready");
        statusBar->setProgress(false);

        wasExporting = false;
    }
}

void AppController::free()
{
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::APP_EXIT, onQuitSubscriptionId);
    TopMenuDispatcher::getInstance().unsubscribe(AppEvents::MENU_EXPORT_MESH, onExportMeshSubscriptionId);

    if (const_cast<AppController*>(this)->exportThread.joinable())
    {
        const_cast<AppController*>(this)->exportThread.join();
    }
}

bool AppController::isExportingMesh() const
{
    return isExporting;
}

void AppController::onQuit()
{
    Eng::Base::getInstance().stop();
}

void AppController::onExportMesh()
{
    TerrainModel* terrain = SetupController::getInstance().getActiveTerrainModel();

    if (!terrain || isExporting.load())
    {
        if (statusBar) statusBar->setMessage("Exporting, wait...");
        return;
    }

    isExporting.store(true);
    wasExporting = true;
    statusBar->setMessage("Exporting...");
    statusBar->setProgress(true, 0.0f);

    TerrainConfig terrainConfig = terrain->getTerrainConfig();

    if (exportThread.joinable()) {
        exportThread.join();
    }

    exportThread = std::thread([
        imgData = terrain->getTerrainImage(),
        size    = terrainConfig.size,
        hScale  = terrainConfig.heightScale,
        &mesh   = *terrain->getTerrainMesh(),
        this]() mutable {
            ObjExporter::exportToObj("./bin/export/terrain.obj", mesh, imgData, size, hScale, &exportProgress);
            isExporting.store(false);
        });
}
