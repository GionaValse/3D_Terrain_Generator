#include <iostream>
#include <vector>
#include "../include/TerrainConfig.hpp"
#include "../include/TerrainGenerator.hpp"
#include "../include/ImageExporter.hpp"
#include "../include/GridGenerator.hpp"
#include <../include/engine/engine.h>

static void renderingLoop(Eng::Node* root) {
}


int main(int argc, char* argv[])
{

    int testResolution = 512;
    Eng::Mesh* gridMesh = terrain::GridGenerator::generate(testResolution);
    Eng::Camera* mainCamera;
    Eng::Node* root;

    glm::mat4 cameraPosition = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 15.0f));
    Eng::PerspectiveCamera* perspectiveCamera = new Eng::PerspectiveCamera("mainPerpectiveCamera", cameraPosition);

    perspectiveCamera->setCameraParams(45.0f, RATIO_16_9, 1.0f, 5000.0f);

    mainCamera = perspectiveCamera;

    glm::mat4 ry = glm::rotate(glm::mat4(1.0f), glm::radians(-15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    mainCamera->setViewMatrix(ry * mainCamera->getMatrix());

    Eng::Base& eng = Eng::Base::getInstance();
    eng.init(&argc, argv, "CG Project");
    eng.setActiveCamera(mainCamera);

    root = new Eng::Node();
    root->setMatrix(glm::mat4(1.0f));
    root->addChild(perspectiveCamera);
    root->addChild(gridMesh);

    eng.start(renderingLoop);

    eng.free();

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

    if (!config.isValid())
    {
        std::cerr << "\nErrore: dimensione, frequenza e ottave devono essere maggiori di zero.\n";
        return 1;
    }

    std::cout << "\nGenerazione in corso con: Dimensione=" << config.size << "x" << config.size
              << ", Frequenza=" << config.frequency
              << ", Ottave=" << config.octaves
              << ", Seed=" << config.seed << "...\n";

    // inizializza generator
    terrain::TerrainGenerator generator(config);

    // generazione mappa
    std::vector<float> image = generator.generate();

    // esportazione img in exr
    if (!terrain::ImageExporter::saveEXR(image, config))
    {
        std::cerr << "Operazione fallita.\n";
        return 1;
    }

    std::cout << "\nOperazione completata.\n";

    std::cout << "\n--- TEST GENERAZIONE GRIGLIA ---\n";

    std::vector<terrain::Vertex> vertices;
    std::vector<unsigned int> indices;

    /*
    std::cout << "Numero di vertici generati: " << vertices.size() << " (Attesi: 9)\n";
    std::cout << "Numero di indici generati: " << indices.size() << " (Attesi: 24, ovvero 8 triangoli)\n\n";

    std::cout << "--- Elenco dei Vertici ---\n";
    for (size_t i = 0; i < vertices.size(); ++i)
    {
        std::cout << "Vertice " << i << ": "
                  << "Pos(" << vertices[i].position.x << ", " << vertices[i].position.y << ", " << vertices[i].position.z << ") | "
                  << "UV(" << vertices[i].texCoords.x << ", " << vertices[i].texCoords.y << ")\n";
    }

    std::cout << "\n--- Elenco degli Indici (Triangoli) ---\n";
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        std::cout << "Triangolo " << (i / 3) << ": "
                  << indices[i] << ", " << indices[i + 1] << ", " << indices[i + 2] << "\n";
    }
    */

    std::cout << "n of triangles" << vertices.size();

    return 0;
}