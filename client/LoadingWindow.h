#pragma once

#include <string>

#include "BaseWindow.h"

class LoadingWindow : public BaseWindow
{
public:
	LoadingWindow();

	void basePosition() override;

	void drawContent() override;

	void setTexts(std::string title, std::string text = "Loading...");

private:
	std::string text;
};
