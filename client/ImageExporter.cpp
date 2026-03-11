#include <filesystem>
#include <iostream>

#include "ImageExporter.hpp"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"

namespace fs = std::filesystem;

namespace terrain {

bool ImageExporter::saveEXR(const std::vector<float>& image, const TerrainConfig& config, const std::string& outputDir) {
    if (!fs::exists(outputDir)) {
        fs::create_directories(outputDir);
        std::cout << "Cartella '" << outputDir << "' creata con successo.\n";
    }

    std::string filename = outputDir + "/perlin_" + std::to_string(config.size) +
                           "_f" + std::to_string(static_cast<int>(config.frequency)) +
                           "_o" + std::to_string(config.octaves) +
                           "_s" + std::to_string(config.seed) + ".exr";

    const char* err = nullptr;
    int ret = SaveEXR(image.data(), config.size, config.size, 3, 0, filename.c_str(), &err);

    if (ret != TINYEXR_SUCCESS) {
        if (err) {
            std::cerr << "Errore EXR: " << err << '\n';
            FreeEXRErrorMessage(err);
        }
        return false;
    }

    std::cout << "Immagine salvata con successo in: " << filename << "\n";
    return true;
}

}