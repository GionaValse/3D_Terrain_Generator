#include <iostream>
#include <vector>
#include <string>

#include "engine.h"

#include "TerrainConfig.hpp"
#include "TerrainGenerator.hpp"
#include "ImageExporter.hpp"
#include "GridGenerator.hpp"

/////////////
// Globals //
/////////////

Eng::Camera* mainCamera;


#ifdef _DEBUG
bool isWireFrameMode = false;
#endif

///////////////
// Callbacks //
///////////////

static void renderingLoop(Eng::Node* root) {
}

static void onKeyboardPressedCallback(unsigned char key, int mouseX, int mouseY) {

#ifdef _DEBUG
    if (key == 'k')
    {
        isWireFrameMode = !isWireFrameMode;
        Eng::Base::getInstance().changeWireFrame(isWireFrameMode);
    }
#endif // _DEBUG

    const float rotSpeed = 2.0f;  // gradi per tasto
    const float moveSpeed = 20.0f; // unità per tasto

    if (key == 'a')
    {
        // Yaw sinistra: ruota attorno all'asse Y del mondo
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'd')
    {
        // Yaw destra
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'w')
    {
        // Avanti sul piano orizzontale (proietta il forward sul piano XZ)
        glm::vec3 forward = -glm::vec3(mainCamera->getMatrix()[2]);
        forward.y = 0.0f;
        forward = glm::normalize(forward) * moveSpeed;
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), forward) * mainCamera->getMatrix());
    }
    else if (key == 's')
    {
        // Indietro sul piano orizzontale
        glm::vec3 forward = -glm::vec3(mainCamera->getMatrix()[2]);
        forward.y = 0.0f;
        forward = glm::normalize(forward) * moveSpeed;
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), -forward) * mainCamera->getMatrix());
    }
    else if (key == 'e')
    {
        // Pitch su: ruota attorno all'asse X locale della camera
        mainCamera->setMatrix(mainCamera->getMatrix() * glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    else if (key == 'q')
    {
        // Pitch giù
        mainCamera->setMatrix(mainCamera->getMatrix() * glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    else if (key == 'r')
    {
        // Su lungo l'asse Y del mondo
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, moveSpeed, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'f')
    {
        // Giù lungo l'asse Y del mondo
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -moveSpeed, 0.0f)) * mainCamera->getMatrix());
    }
}

//////////
// Main //
//////////

int main(int argc, char* argv[])
{
    unsigned int testSize = 512;
    float testResolution = 1.0f;

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

    float heightScale;
    std::cout << "Inserisci l'altezza del terreno (es. 50.0 o 1000.0): ";
    std::cin >> heightScale;

    if (!config.isValid() || heightScale <= 0.0f)
    {
        std::cerr << "\nErrore: dimensione, frequenza, ottave e altezza devono essere maggiori di zero.\n";
        return 1;
    }

    std::cout << "\nGenerazione in corso con: Dimensione=" << config.size << "x" << config.size
        << ", Frequenza=" << config.frequency
        << ", Ottave=" << config.octaves
        << ", Seed=" << config.seed
        << ", Altezza=" << heightScale << "...\n";

    // inizializza generator
    terrain::TerrainGenerator generator(config);

    // generazione mappa
    std::vector<float> image = generator.generate();

    // esportazione img in exr
    std::string textureName;
    if (!terrain::ImageExporter::saveEXR(image, config, textureName))
    {
        std::cerr << "Operazione fallita.\n";
        return 1;
    }

    std::cout << "\nOperazione completata.\n";

    std::cout << "\n--- TEST GENERAZIONE GRIGLIA ---\n";

    Eng::Base& eng = Eng::Base::getInstance();
    eng.init(&argc, argv, "Terrain Test");

    // Terrain shader
    Eng::Shader* vShader = new Eng::Shader();
    Eng::Shader* fShader = new Eng::Shader();
    vShader->loadFromFile(Eng::Shader::TYPE_VERTEX, "./shaders/terrain.vert");
    fShader->loadFromFile(Eng::Shader::TYPE_FRAGMENT, "./shaders/terrain.frag");

    Eng::Shader* terrainShader = new Eng::Shader();
    terrainShader->build(vShader, fShader);
    terrainShader->render();
    terrainShader->setFloat(terrainShader->getParamLocation("heightScale"), heightScale);

    // Preparing material and texture
    Eng::Texture* heightMap = new Eng::Texture("TerrainHeightMap", config.size, config.size, image);
    Eng::Material* material = new Eng::Material("TerrainMaterial");

    material->setShader(terrainShader);
    material->setTexture(heightMap);

    Eng::Mesh* gridMesh = terrain::GridGenerator::generate(testSize, testResolution);
    gridMesh->setMatrix(glm::mat4(1.0f));

    gridMesh->setMaterial(material);

    glm::mat4 camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 15.0f));
    Eng::PerspectiveCamera* perspectiveCamera = new Eng::PerspectiveCamera("mainCamera", camera);
    perspectiveCamera->setCameraParams(45.0f, RATIO_16_9, 1.0f, 5000.0f);

    Eng::InfiniteLight* sun = new Eng::InfiniteLight("sun");
    sun->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    sun->setAmbient(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    sun->setDiffuse(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    sun->setSpecular(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    Eng::Node* root = eng.getSceneGraphInstance();
    root->addChild(sun);
    root->addChild(gridMesh);
    eng.setActiveCamera(perspectiveCamera);

    mainCamera = perspectiveCamera;

    eng.setOnKeyboardPressedCallback(onKeyboardPressedCallback);

    eng.start(renderingLoop);

    return 0;
}