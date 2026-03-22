#include "LoadingWindow.h"

LoadingWindow::LoadingWindow() 
	: BaseWindow(
		"Loading", 
		true, 
		false,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse
	),
	text("Loading...")
{
}

void LoadingWindow::basePosition()
{
	ImGui::SetNextWindowPos(defaultPos, posCond, ImVec2(0.5f, 0.5f));

	setNextWindowPos(
		ImGui::GetMainViewport()->GetCenter().x,
		ImGui::GetMainViewport()->GetCenter().y,
		ImGuiCond_Always
	);
}

void LoadingWindow::drawContent()
{
	ImGui::Text(text.c_str());

	static float anim = 0.0f;
	anim += ImGui::GetIO().DeltaTime;
	ImGui::ProgressBar(-1.0f * anim, ImVec2(200, 0));
}

void LoadingWindow::setTexts(std::string title, std::string text)
{
	this->title = title;
	this->text = text;
}
