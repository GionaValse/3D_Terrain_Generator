#pragma once

#include "engine.h"

#include "TerrainConfig.h"

class TerrainGenerator
{
public:
	explicit TerrainGenerator(const TerrainConfig& config);

	Eng::Mesh* generate() const;

private:
	TerrainConfig m_config;
};

