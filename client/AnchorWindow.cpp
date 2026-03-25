#include "AnchorWindow.h"

AnchorWindow::AnchorWindow(const std::string& name, bool startVisible, bool cancelable, ImGuiWindowFlags windowFlags)
	: BaseWindow(name, startVisible,cancelable, windowFlags)
{}

void AnchorWindow::basePosition()
{
	ImGui::SetNextWindowPos(defaultPos, posCond, ImVec2(1.0f, 0.5f));

	setNextWindowPos(
		ImGui::GetMainViewport()->WorkSize.x,
		ImGui::GetMainViewport()->GetCenter().y,
		ImGuiCond_Always
	);
}
