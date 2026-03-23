#include "SetupWindow.h"

SetupWindow::SetupWindow()
	: CentredWindow("Terrain Setup"),
	heightScale(100.0f),
	triggerGeneration(false)
{
	config.size = 512;
	config.frequency = 4.0f;
	config.octaves = 6;
	config.seed = 12345;
}

void SetupWindow::drawContent()
{
	ImGui::Text("Noise Parameters");
	ImGui::InputInt("Size (px)", (int*)&config.size);
	ImGui::SliderFloat("Frequency", &config.frequency, 0.1f, 20.0f);
	ImGui::SliderInt("Octaves", &config.octaves, 1, 10);
	ImGui::InputScalar("Seed", ImGuiDataType_U32, &config.seed);

	ImGui::Separator();
	ImGui::Text("Mesh Parameters");
	ImGui::DragFloat("Height Scale", &heightScale, 1.0f, 0.1f, 5000.0f);

	if (ImGui::Button("Generate New Terrain", ImVec2(-1, 0))) {
		triggerGeneration = true;
	}
}

terrain::TerrainConfig SetupWindow::getTerrainConfiguartion() const
{
	return config;
}

float SetupWindow::getHeightScale() const
{
	return heightScale;
}

bool SetupWindow::checkAndResetTrigger()
{
	bool val = triggerGeneration;
	triggerGeneration = false;
	return val;
}
