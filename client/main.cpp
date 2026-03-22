#include <iostream>
#include <vector>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

#include "TerrainGenerator.hpp"
#include "ImageExporter.hpp"
#include "GridGenerator.hpp"
#include "SetupWindow.h"
#include "EditViewWindow.h"

/////////////
// Globals //
/////////////

Eng::Camera* mainCamera = nullptr;
Eng::Shader* terrainShader = nullptr;
Eng::Texture* heightMap = nullptr;

#ifdef _DEBUG
bool isWireFrameMode = false;
#endif

SetupWindow* g_SetupWin = nullptr;
EditViewWindow* g_EditViewWin = nullptr;

///////////////////////
// Terrain Generator //
///////////////////////

static void generateTerrain(terrain::TerrainConfig config, float heightScale)
{
    terrain::TerrainGenerator generator(config);
    std::vector<float> image = generator.generate();
    std::string textureName;

    if (!terrain::ImageExporter::saveEXR(image, config, textureName))
    {
        std::cerr << "Operazione fallita.\n";
        return;
    }

    std::cout << "\nOperazione completata.\n";

    std::cout << "\n--- TEST GENERAZIONE GRIGLIA ---\n";

    terrainShader->setFloat(terrainShader->getParamLocation("heightScale"), heightScale);

    if (heightMap) delete heightMap;
    heightMap = new Eng::Texture("TerrainHeightMap", config.size, config.size, image);
}

///////////////
// Callbacks //
///////////////

void onResize(int w, int h) {
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}

static void renderingLoop(Eng::Node* root)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
}

static void renderingImGui(Eng::GUIObjects obj)
{
    if (g_SetupWin) g_SetupWin->render();
    if (g_EditViewWin) g_EditViewWin->render();

    if (g_SetupWin && g_SetupWin->checkAndResetTrigger()) {
        generateTerrain(g_SetupWin->getTerrainConfiguartion(), g_SetupWin->getHeightScale());
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

static void onSpecialKeyCallback(int key, int x, int y) {
    ImGui_ImplGLUT_SpecialFunc(key, x, y);
}

static void onKeyboardPressedCallback(unsigned char key, int mouseX, int mouseY) 
{
    ImGuiIO& io = ImGui::GetIO();

    if (key >= 32)
    {
        io.AddInputCharacter((unsigned int)key);
    }

    io.AddKeyEvent(ImGuiKey_Backspace, (key == 8));
    io.AddKeyEvent(ImGuiKey_Enter, (key == 13));
    io.AddKeyEvent(ImGuiKey_Escape, (key == 27));

    if (ImGui::GetIO().WantCaptureKeyboard) 
    {
        return; // Write on interface inputs
    }

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
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'd')
    {
        // Yaw destra
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
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

static void onMouseCallback(int buttonId, int buttonState, int mouseX, int mouseY)
{
    ImGui_ImplGLUT_MouseFunc(buttonId, buttonState, mouseX, mouseY);

    if (ImGui::GetIO().WantCaptureMouse)
    {
        return; // Use mouse on interface
    }
}

//////////
// Main //
//////////

int main(int argc, char* argv[])
{
    // ----- SETTING UP ENGINE ----- //

    Eng::Base& eng = Eng::Base::getInstance();
    eng.init(&argc, argv, "3D Terrain Editor");

    // ----- SETTING UP IMGUI ----- //

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();

    ImGui_ImplOpenGL3_Init("#version 440");

    g_SetupWin = new SetupWindow();
    g_EditViewWin = new EditViewWindow();

    // ----- SETTING UP SCENE ----- //

    // Initilize Terrain
    Eng::Shader* vShader = new Eng::Shader();
    Eng::Shader* fShader = new Eng::Shader();
    vShader->loadFromFile(Eng::Shader::TYPE_VERTEX, "./shaders/terrain.vert");
    fShader->loadFromFile(Eng::Shader::TYPE_FRAGMENT, "./shaders/terrain.frag");

    terrainShader = new Eng::Shader();
    terrainShader->build(vShader, fShader);
    terrainShader->render();

    Eng::Material* material = new Eng::Material("TerrainMaterial");

    material->setShader(terrainShader);
    material->setTexture(heightMap);

    Eng::Mesh* gridMesh = terrain::GridGenerator::generate(512, 1.0f);
    gridMesh->setMatrix(glm::mat4(1.0f));

    gridMesh->setMaterial(material);

    // Initilize Camera
    glm::mat4 camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 15.0f));
    Eng::PerspectiveCamera* perspectiveCamera = new Eng::PerspectiveCamera("mainCamera", camera);
    perspectiveCamera->setCameraParams(45.0f, RATIO_16_9, 1.0f, 5000.0f);

    // Initilize Sun light
    Eng::InfiniteLight* sun = new Eng::InfiniteLight("sun");
    sun->setDirection(glm::vec3(-0.5f, -1.0f, -0.3f));
    sun->setAmbient(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    sun->setDiffuse(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    sun->setSpecular(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Setup scene
    Eng::Node* root = eng.getSceneGraphInstance();
    root->addChild(perspectiveCamera);
    root->addChild(sun);
    root->addChild(gridMesh);

    mainCamera = perspectiveCamera;
    eng.setActiveCamera(mainCamera);

    // ----- ENGINE START ----- //

    eng.setOnResizeCallback(onResize);
    eng.setOnSpecialPressedCallback(onSpecialKeyCallback);
    eng.setOnKeyboardPressedCallback(onKeyboardPressedCallback);
    eng.setOnMouseCallback(onMouseCallback);
    eng.setOnTextDrawCallback(renderingImGui);

    eng.start(renderingLoop);


    // ----- FREE APPLICATION ----- //

    eng.free();

    delete g_SetupWin;
    delete g_EditViewWin;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}