#pragma once

#include <string>

#include "CentredWindow.h"

class LoadingWindow : public CentredWindow
{
public:
	LoadingWindow();

	void drawContent() override;

	void setTexts(std::string title, std::string text = "Loading...");

private:
	std::string text;
};
