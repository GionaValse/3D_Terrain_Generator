#include "ToolWindow.h"

ToolWindow::ToolWindow() : AnchorWindow("ToolWindow")
{}

void ToolWindow::drawContent()
{
	ImGui::Text("Porcodio");
}
