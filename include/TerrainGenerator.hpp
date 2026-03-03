#pragma once
#include "TerrainConfig.hpp"
#include "PerlinNoise.hpp"
#include <vector>

namespace terrain
{

    class TerrainGenerator
    {
    public:
        explicit TerrainGenerator(const TerrainConfig &config);

        std::vector<float> generate() const;

    private:
        TerrainConfig m_config;
        siv::PerlinNoise m_perlin;
    };

}