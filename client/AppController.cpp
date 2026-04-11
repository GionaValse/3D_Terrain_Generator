#include "AppController.h"

#include "AppEvents.h"
#include "ObjExporter.hpp"

#include "engine.h"

AppController::AppController()
    : topMenuBar(nullptr),
    statusBar(nullptr),
    m_config(nullptr),
    isExporting(false),
    gridMesh(nullptr),
    onQuitSubscriptionId(-1),
    onExportMeshSubscriptionId(-1)
{}

AppController& AppController::getInstance()
{
    static AppController instance;
    return instance;
}

void AppController::init(BaseWindow* topMenuBar, StatusBar* statusBar, ConfigModel* config)
{
    this->topMenuBar = topMenuBar;
    this->statusBar = statusBar;
    this->m_config = config;

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

void AppController::setTerrain(std::vector<float> imageArray, Eng::Mesh* terrainMesh)
{
    this->image = imageArray;
    this->gridMesh = terrainMesh;
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
    if (isExporting.load())
    {
        statusBar->setMessage("Exporting, wait...");
        return;
    }

    isExporting.store(true);
    wasExporting = true;
    statusBar->setMessage("Exporting...");
    statusBar->setProgress(true, 0.0f);

    TerrainConfig terrainConfiguration = m_config->getActiveTerrainConfig();

    if (exportThread.joinable()) {
        exportThread.join();
    }

    exportThread = std::thread([
        imgData = m_config->getHeightMapImage(),
        size = terrainConfiguration.size,
        hScale = terrainConfiguration.heightScale,
        &mesh = *gridMesh // Do not edit the orginal mash until exporting done
        , this]() mutable {
            ObjExporter::exportToObj("./bin/export/terrain.obj", mesh, imgData, size, hScale, &exportProgress);
            isExporting.store(false);
        });
}
