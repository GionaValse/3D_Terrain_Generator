#include "EditViewWindow.h"

EditViewWindow::EditViewWindow() : BaseWindow("Terrain View")
{
}

void EditViewWindow::drawContent()
{
    Eng::Base& eng = getEngineInstance();

    ImGui::Text("Engine Status: Running");
    ImGui::Separator();

    static bool wireframe = false;
    if (ImGui::Checkbox("Wireframe Mode", &wireframe)) {
        eng.changeWireFrame(wireframe);
    }

    ImGui::Text(
        "Application average %.3f ms/frame (%.1f FPS)",
        1000.0f / ImGui::GetIO().Framerate, 
        ImGui::GetIO().Framerate
    );

    if (ImGui::Button("Reset Camera")) {
    }
}

Eng::Base& EditViewWindow::getEngineInstance()
{
	return Eng::Base::getInstance();
}
