#include "LoadingWindow.h"

LoadingWindow::LoadingWindow() 
	: CentredWindow("Loading"),
	text("Loading...")
{
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
