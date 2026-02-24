#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

#include "PerlinNoise.hpp"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

namespace fs = std::filesystem;

int main() {
    int size;
    double frequency;
    int octaves;
    uint32_t seed;

    std::cout << "--- Generatore di Perlin Noise EXR ---\n\n";

    std::cout << "Inserisci la dimensione dell'immagine (es. 512 o 1024): ";
    std::cin >> size;

    std::cout << "Inserisci la frequenza (es. 4.0 o 8.5): ";
    std::cin >> frequency;

    std::cout << "Inserisci il numero di ottave (es. 4 o 6): ";
    std::cin >> octaves;

    std::cout << "Inserisci il seed (es. 123456): ";
    std::cin >> seed;

    if (size <= 0 || octaves <= 0 || frequency <= 0) {
        std::cerr << "\nErrore: dimensione, frequenza e ottave devono essere maggiori di zero.\n";
        return 1;
    }

    const int width = size;
    const int height = size;

    std::cout << "\nGenerazione in corso con: Dimensione=" << size << "x" << size
              << ", Frequenza=" << frequency
              << ", Ottave=" << octaves
              << ", Seed=" << seed << "...\n";

    const siv::PerlinNoise perlin{seed};
    std::vector<float> image(width * height * 3);

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            double nx = static_cast<double>(x) / width;
            double ny = static_cast<double>(y) / height;

            double noiseValue = perlin.octave2D_01(nx * frequency, ny * frequency, octaves);

            int pixelIndex = (y * width + x) * 3;
            image[pixelIndex + 0] = static_cast<float>(noiseValue);
            image[pixelIndex + 1] = static_cast<float>(noiseValue);
            image[pixelIndex + 2] = static_cast<float>(noiseValue);
        }
    }

    std::string outputDir = "bin/noises";

    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
        std::cout << "Cartella '" << outputDir << "' creata con successo.\n";
    }

    std::string filename = outputDir + "/perlin_" + std::to_string(size) +
                           "_f" + std::to_string(static_cast<int>(frequency)) +
                           "_o" + std::to_string(octaves) +
                           "_s" + std::to_string(seed) + ".exr";

    const char* err = nullptr;
    int ret = SaveEXR(image.data(), width, height, 3, 0, filename.c_str(), &err);

    if (ret != TINYEXR_SUCCESS) {
        if (err) {
            std::cerr << "Errore EXR: " << err << '\n';
            FreeEXRErrorMessage(err);
        }
        return 1;
    }

    std::cout << "Immagine salvata con successo in: " << filename << "\n\n";
    return 0;
}
