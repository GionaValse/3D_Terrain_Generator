#include "../include/TerrainGenerator.hpp"
#include <cmath>
#include <algorithm>

namespace terrain {

TerrainGenerator::TerrainGenerator(const TerrainConfig& config)
    : m_config(config), m_perlin(config.seed) {}

std::vector<float> TerrainGenerator::generate() const {
    const int width = m_config.size;
    const int height = m_config.size;
    std::vector<float> image(width * height * 3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double nx = static_cast<double>(x) / width;
            double ny = static_cast<double>(y) / height;

            //parte montagne
            double macroFrequency = m_config.frequency * 1.0; 
            double macroAmplifier = 10.0; 
            double macroNoise = m_perlin.noise2D(nx * macroFrequency, ny * macroFrequency) * macroAmplifier;

            //parte rocce
            double microFrequency = m_config.frequency * 4.0; 
            double microAmplifier = 1.0;
            double microNoise = m_perlin.normalizedOctave2D(nx * microFrequency, ny * microFrequency, m_config.octaves) * microAmplifier;

            //combinazione
            double combinedNoise = macroNoise + microNoise;
            double realisticMax = (macroAmplifier + microAmplifier) * 0.65;
            double noise_11 = combinedNoise / realisticMax;
            double noiseValue = (noise_11 * 0.5) + 0.5;  

            //normalizzato
            noiseValue = std::clamp(noiseValue, 0.0, 1.0);
            noiseValue = std::pow(noiseValue, 1.5); 

            //grigio
            int pixelIndex = (y * width + x) * 3;
            image[pixelIndex + 0] = static_cast<float>(noiseValue);
            image[pixelIndex + 1] = static_cast<float>(noiseValue);
            image[pixelIndex + 2] = static_cast<float>(noiseValue);
        }
    }

    return image;
}

}