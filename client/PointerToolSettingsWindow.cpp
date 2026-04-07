#include "PointerToolSettingsWindow.h"

PointerToolSettingsWindow::PointerToolSettingsWindow()
	: CentredWindow("PointerTool Settings", false),
	IToolSettingsWindow(),
	currentTool(nullptr),
    radius(0),
    strength(0.0f),
    falloff(0.0f)
{}

PointerToolSettingsWindow::~PointerToolSettingsWindow()
{}

void PointerToolSettingsWindow::drawContent()
{
    if (!currentTool)
    {
        ImGui::TextDisabled("No tool selected.");
        return;
    }

    ImGui::Text("Tool: %s", currentTool->getName().c_str());
    ImGui::Separator();
    ImGui::Spacing();

    if (ImGui::SliderInt("Radius", &radius, 1, 100))
    {
        currentTool->setRadius(radius);
    }

    if (ImGui::SliderFloat("Strength", &strength, 0.001f, 1.0f, "%.3f"))
    {
        currentTool->setStrength(strength / 10.0f);
    }

    if (ImGui::SliderFloat("Falloff", &falloff, 0.0f, 5.0f, "%.2f"))
    {
        currentTool->setFalloff(falloff);
    }

    if (ImGui::Button("Close", ImVec2(-1.0f, 0.0f)))
    {
        saveAndDismiss();
    }
}

void PointerToolSettingsWindow::setVisible(bool visible)
{
	CentredWindow::setVisible(visible);
}

void PointerToolSettingsWindow::setActiveTool(BaseBrushTool * tool)
{
	this->currentTool = tool;

    this->radius = this->currentTool->getRadius();
    this->strength = this->currentTool->getStrength() * 10.0f;
    this->falloff = this->currentTool->getFalloff();
}

void PointerToolSettingsWindow::saveAndDismiss()
{
	if (!this->currentTool) return;
    this->setVisible(false);
}
