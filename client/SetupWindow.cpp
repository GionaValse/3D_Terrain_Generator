#include "SetupWindow.h"

#include <imgui.h>

#include "configuration.h"

SetupWindow::SetupWindow()
	: CentredWindow("Texture Setup", false),
	m_listener(nullptr)
{
	init();
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
	ImGui::DragFloat("Height Scale", &m_terrainConfig.heightScale, 1.0f, 0.1f, 5000.0f);
	
	if (ImGui::Button("Generate New Terrain", ImVec2(-1, 0))) {
		if (m_listener) {
			m_listener->onTerrainGenerationRequest(m_terrainConfig, m_textureConfig);
		}
	}
}

void SetupWindow::init()
{
	m_textureConfig.size = TERRAIN_BASE_SIZE;
	m_textureConfig.frequency = TERRAIN_BASE_FREQUENCY;
	m_textureConfig.octaves = TERRAIN_BASE_OCTAVES;
	m_textureConfig.seed = TERRAIN_BASE_SEED;

	m_terrainConfig.size = TERRAIN_BASE_SIZE;
	m_terrainConfig.heightScale = TERRAIN_BASE_HEIGHTSCALE;
}
