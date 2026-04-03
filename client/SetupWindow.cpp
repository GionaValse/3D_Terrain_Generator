#include "SetupWindow.h"
#include <imgui.h>

SetupWindow::SetupWindow(TextureConfig& textureConfig, TerrainConfig& terrainConfig)
	: CentredWindow("Texture Setup"),
	m_textureConfig(textureConfig),
	m_terrainConfig(terrainConfig),
	m_listener(nullptr),
	heightScale(100.0f) 
{
	m_textureConfig.size = 512;
	m_textureConfig.frequency = 4.0f;
	m_textureConfig.octaves = 6;
	m_textureConfig.seed = 12345;
}

void SetupWindow::setListener(ISetupListener* listener)
{
	m_listener = listener;
}

void SetupWindow::drawContent()
{
	ImGui::Text("Noise Parameters");
	ImGui::InputInt("Size (px)", (int*)&m_textureConfig.size);
	ImGui::SliderFloat("Frequency", &m_textureConfig.frequency, 0.1f, 20.0f);
	ImGui::SliderInt("Octaves", &m_textureConfig.octaves, 1, 10);
	ImGui::InputScalar("Seed", ImGuiDataType_U32, &m_textureConfig.seed);

	ImGui::Separator();
	ImGui::Text("Mesh Parameters");
	ImGui::InputInt("Size", (int*)&m_terrainConfig.size);
	ImGui::DragFloat("Height Scale", &heightScale, 1.0f, 0.1f, 5000.0f);

	
	if (ImGui::Button("Generate New Terrain", ImVec2(-1, 0))) {
		if (m_listener) {
			m_listener->onTerrainGenerationRequested();
		}
	}
}

float SetupWindow::getHeightScale() const
{
	return heightScale;
}
