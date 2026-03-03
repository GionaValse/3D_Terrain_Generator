#include <iostream>
#include "TerrainConfig.hpp"
#include "TerrainGenerator.hpp"
#include "ImageExporter.hpp"

int main() {
    terrain::TerrainConfig config;

    std::cout << "--- Generatore di Perlin Noise EXR ---\n\n";

    std::cout << "Inserisci la dimensione dell'immagine (es. 512 o 1024): ";
    std::cin >> config.size;

    std::cout << "Inserisci la frequenza (es. 4.0 o 8.5): ";
    std::cin >> config.frequency;

    std::cout << "Inserisci il numero di ottave (es. 4 o 6): ";
    std::cin >> config.octaves;

    std::cout << "Inserisci il seed (es. 123456): ";
    std::cin >> config.seed;

    if (!config.isValid()) {
        std::cerr << "\nErrore: dimensione, frequenza e ottave devono essere maggiori di zero.\n";
        return 1;
    }

    std::cout << "\nGenerazione in corso con: Dimensione=" << config.size << "x" << config.size
              << ", Frequenza=" << config.frequency
              << ", Ottave=" << config.octaves
              << ", Seed=" << config.seed << "...\n";

    // inizializza generator
    terrain::TerrainGenerator generator(config);

    //generazione mappa
    std::vector<float> image = generator.generate();

    //esportazione img in exr
    if (!terrain::ImageExporter::saveEXR(image, config)) {
        std::cerr << "Operazione fallita.\n";
        return 1;
    }

    std::cout << "\nOperazione completata.\n";
    return 0;
}