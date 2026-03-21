#include <algorithm>
#include <cmath>

#include "TerrainGenerator.hpp"

namespace terrain {

    TerrainGenerator::TerrainGenerator(const TerrainConfig& config)
        : m_config(config), m_perlin(config.seed) {
    }

    std::vector<float> TerrainGenerator::generate() const {
        const int width = m_config.size;
        const int height = m_config.size;
        std::vector<float> image(width * height * 3);

        const double macroAmplifier = 10.0;
        const double microAmplifier = 1.0;

        const double macroFrequency = m_config.frequency;
        const double microFrequency = m_config.frequency * 4.0;

        const double macroWeight = 0.85;
        const double microWeight = 0.15;

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                double nx = static_cast<double>(x) / width;
                double ny = static_cast<double>(y) / height;

                double macroNoise = m_perlin.noise2D(nx * macroFrequency, ny * macroFrequency) * macroAmplifier;

                double microNoise = m_perlin.normalizedOctave2D(nx * microFrequency, ny * microFrequency, m_config.octaves) * microAmplifier;

                double macro01 = (macroNoise + macroAmplifier) / (2.0 * macroAmplifier);
                double micro01 = (microNoise + microAmplifier) / (2.0 * microAmplifier);

                double noiseValue = macro01 * macroWeight + micro01 * microWeight;

                noiseValue = std::pow(noiseValue, 1.5);

                int pixelIndex = (y * width + x) * 3;
                image[pixelIndex + 0] = static_cast<float>(noiseValue);
                image[pixelIndex + 1] = static_cast<float>(noiseValue);
                image[pixelIndex + 2] = static_cast<float>(noiseValue);
            }
        }

        return image;
    }

}