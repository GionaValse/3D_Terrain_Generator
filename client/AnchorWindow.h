#pragma once

#include "BaseWindow.h"

class AnchorWindow : public BaseWindow
{
public:
	AnchorWindow(
		const std::string& name = "",
		bool startVisible = true,
		bool cancelable = false,
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse
	);

	void basePosition() override;
};
