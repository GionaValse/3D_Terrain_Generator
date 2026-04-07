#include "UIController.h"

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

UIController::UIController()
    : topMenuBar(nullptr),
    statusBar(nullptr)
{}

UIController& UIController::getInstance()
{
    static UIController instance;
    return instance;
}

void UIController::init(std::vector<BaseWindow*> windows, BaseWindow* topMenuBar, StatusBar* statusBar)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 440");

	this->windows = windows;
	this->topMenuBar = topMenuBar;
	this->statusBar = statusBar;

	setupTheme();
}

void UIController::free()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    if (this->topMenuBar)
    {
        delete this->topMenuBar;
        this->topMenuBar = nullptr;
    }

    if (this->statusBar)
    {
        delete this->statusBar;
        this->statusBar = nullptr;
	}

    for (auto window : this->windows)
    {
        if (window)
        {
            delete window;
        }
    }
	this->windows.clear();
}

void UIController::prepareFrame()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
}

void UIController::render()
{
    renderDockSpace();

    for (auto window : this->windows)
    {
        if (window && window->isOpen())
        {
            window->render();
        }
	}
}

void UIController::onResize(int width, int height)
{
	if (width <= 0 || height <= 0) return;

    if (this->topMenuBar)
    {
        this->topMenuBar->onResize(width, height);
	}

    if (this->statusBar)
    {
        this->statusBar->onResize(width, height);
    }

    for (auto window : this->windows)
    {
        if (window)
        {
            window->onResize(width, height);
        }
	}
}

void UIController::setupTheme()
{
	// If we want to customize the theme, we can do it here by modifying ImGui::GetStyle() and ImGui::GetStyle().Colors
}

void UIController::renderDockSpace()
{
    if (this->topMenuBar)
    {
        this->topMenuBar->render();
	}

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;
    ImGui::DockSpaceOverViewport(0U, viewport, dockspaceFlags);

    if (this->statusBar)
    {
        Eng::Camera* camera = Eng::Base::getInstance().getActiveCamera();
        int fps = Eng::Base::getInstance().getCurrentFPS();

        this->statusBar->updateStats(camera->getMatrix()[3], fps);
        this->statusBar->render();
	}
}
