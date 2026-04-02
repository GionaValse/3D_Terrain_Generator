#include "SetupWindow.h"

SetupWindow::SetupWindow()
	: CentredWindow("Texture Setup"),
	heightScale(100.0f),
	triggerGeneration(false)
{
	TextureConfig& textureConfig = ConfigController::getInstance().getActiveTextureConfig();

	textureConfig.size = 512;
	textureConfig.frequency = 4.0f;
	textureConfig.octaves = 6;
	textureConfig.seed = 12345;
}

void SetupWindow::drawContent()
{
	TextureConfig& textureConfig = ConfigController::getInstance().getActiveTextureConfig();
	TerrainConfig& terrainConfig = ConfigController::getInstance().getActiveTerrainConfig();
	ImGui::Text("Noise Parameters");
	ImGui::InputInt("Size (px)", (int*)&textureConfig.size);
	ImGui::SliderFloat("Frequency", &textureConfig.frequency, 0.1f, 20.0f);
	ImGui::SliderInt("Octaves", &textureConfig.octaves, 1, 10);
	ImGui::InputScalar("Seed", ImGuiDataType_U32, &textureConfig.seed);

	ImGui::Separator();
	ImGui::Text("Mesh Parameters");
	ImGui::InputInt("Size", (int*)&terrainConfig.size);
	ImGui::DragFloat("Height Scale", &heightScale, 1.0f, 0.1f, 5000.0f);

	if (ImGui::Button("Generate New Terrain", ImVec2(-1, 0))) {
		triggerGeneration = true;
	}
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
