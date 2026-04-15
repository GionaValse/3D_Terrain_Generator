#pragma once

#include "engine.h"

#include "TerrainConfig.h"

class TerrainGenerator
{
public:
	explicit TerrainGenerator(const TerrainConfig& config);

	std::vector<Eng::Mesh*> generate(int& gridDimX, int& gridDimZ, std::atomic<float>* progress = nullptr) const;

private:
	TerrainConfig m_config;

	Eng::Mesh* generateChunk(int startX, int startZ, int chunkSize, int globalSize) const;
};

