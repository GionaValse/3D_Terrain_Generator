#include <iostream>
#include <vector>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

#include "TerrainGenerator.hpp"
#include "ImageExporter.hpp"
#include "ObjExporter.hpp"
#include "GridGenerator.hpp"
#include "SetupWindow.h"
#include "EditViewWindow.h"

/////////////
// Globals //
/////////////

Eng::Camera* mainCamera = nullptr;
Eng::Mesh* gridMesh = nullptr;
Eng::Shader* terrainShader = nullptr;
Eng::Texture* heightMap = nullptr;
Eng::InfiniteLight* sunLight = nullptr;

#ifdef _DEBUG
bool isWireFrameMode = false;
#endif

SetupWindow* g_SetupWin = nullptr;
EditViewWindow* g_EditViewWin = nullptr;

std::vector<float> image;

// Sun parameters
float sunAzimuth = glm::radians(210.0f);
float sunElevation = glm::radians(45.0f);
glm::vec3 sunPosition(0.0f);
bool sunRotating = false;

void updateSunDirection()
{
    glm::vec3 dir;
    dir.x = cos(sunElevation) * sin(sunAzimuth);
    dir.y = -sin(sunElevation);
    dir.z = cos(sunElevation) * cos(sunAzimuth);
    sunLight->setDirection(glm::normalize(dir));

    sunPosition = -glm::normalize(dir) * 300.0f;
}

///////////////////////
// Terrain Generator //
///////////////////////

static void generateTerrain(terrain::TerrainConfig config, float heightScale)
{
    terrain::TerrainGenerator generator(config);
    image.clear();
    image = generator.generate();
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

static void exportTerrain()
{
    terrain::TerrainConfig config = g_SetupWin->getTerrainConfiguartion();
    float heightScale = g_SetupWin->getHeightScale();

    ObjExporter::exportToObj(
        "./bin/export/terrain.obj",
        *gridMesh,
        image,
        config.size,
        heightScale
    );
}

///////////////
// Callbacks //
///////////////

void onResize(int w, int h) {
    ImGui_ImplGLUT_ReshapeFunc(w, h);
}

static void renderingLoop(Eng::Node* root)
{
    // Auto-rotate sun
    if (sunRotating)
    {
        sunAzimuth += glm::radians(0.5f);
        updateSunDirection();
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGLUT_NewFrame();
    ImGui::NewFrame();
}

static void renderingImGui(Eng::GUIObjects obj)
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Nuovo", "Ctrl+N")) { /* Logica */ }
            if (ImGui::MenuItem("Salva EXR", "Ctrl+S")) { /* Logica */ }
            if (ImGui::MenuItem("Esporta terreno", "Ctrl+E")) {
                exportTerrain();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Esci", "Alt+F4")) { /* Logica */ }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Modifica"))
        {
            if (ImGui::MenuItem("Reset Camera")) { /* Logica */ }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (g_SetupWin) g_SetupWin->render();
    if (g_EditViewWin) g_EditViewWin->render();

    if (g_SetupWin && g_SetupWin->checkAndResetTrigger()) {
        generateTerrain(g_SetupWin->getTerrainConfiguartion(), g_SetupWin->getHeightScale());
    }

    // Draw sun indicator on screen
    {
        glm::mat4 view = glm::inverse(mainCamera->getMatrix());
        glm::mat4 proj = mainCamera->getProjectionMatrix();

        glm::vec4 clip = proj * view * glm::vec4(sunPosition, 1.0f);

        if (clip.w > 0.0f)
        {
            glm::vec3 ndc = glm::vec3(clip) / clip.w;
            ImVec2 screen;
            screen.x = (ndc.x * 0.5f + 0.5f) * ImGui::GetIO().DisplaySize.x;
            screen.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * ImGui::GetIO().DisplaySize.y;

            ImGui::GetBackgroundDrawList()->AddCircleFilled(screen, 8.0f, IM_COL32(255, 230, 50, 255));
        }
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
        return;
    }

#ifdef _DEBUG
    if (key == 'v')
    {
        isWireFrameMode = !isWireFrameMode;
        Eng::Base::getInstance().changeWireFrame(isWireFrameMode);
    }
#endif

    const float rotSpeed = 2.0f;
    const float moveSpeed = 20.0f;

    // Camera controls
    if (key == 'a')
    {
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'd')
    {
        mainCamera->setMatrix(glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(0.0f, 1.0f, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'w')
    {
        glm::vec3 forward = -glm::vec3(mainCamera->getMatrix()[2]);
        forward.y = 0.0f;
        forward = glm::normalize(forward) * moveSpeed;
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), forward) * mainCamera->getMatrix());
    }
    else if (key == 's')
    {
        glm::vec3 forward = -glm::vec3(mainCamera->getMatrix()[2]);
        forward.y = 0.0f;
        forward = glm::normalize(forward) * moveSpeed;
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), -forward) * mainCamera->getMatrix());
    }
    else if (key == 'e')
    {
        mainCamera->setMatrix(mainCamera->getMatrix() * glm::rotate(glm::mat4(1.0f), glm::radians(rotSpeed), glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    else if (key == 'q')
    {
        mainCamera->setMatrix(mainCamera->getMatrix() * glm::rotate(glm::mat4(1.0f), glm::radians(-rotSpeed), glm::vec3(1.0f, 0.0f, 0.0f)));
    }
    else if (key == 'r')
    {
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, moveSpeed, 0.0f)) * mainCamera->getMatrix());
    }
    else if (key == 'f')
    {
        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -moveSpeed, 0.0f)) * mainCamera->getMatrix());
    }
    // Toggle sun rotation
    else if (key == 'g')
    {
        sunRotating = !sunRotating;
    }
}

static void onMouseCallback(int buttonId, int buttonState, int mouseX, int mouseY)
{
    ImGui_ImplGLUT_MouseFunc(buttonId, buttonState, mouseX, mouseY);

    if (ImGui::GetIO().WantCaptureMouse)
    {
        return;
    }
}

static void onMouseMotionCallback(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
}

static void onPassiveMouseMotionCallback(int x, int y) {
    ImGui_ImplGLUT_MotionFunc(x, y);
}

//////////
// Main //
//////////

int main(int argc, char* argv[])
{
    Eng::Base& eng = Eng::Base::getInstance();
    eng.init(&argc, argv, "3D Terrain Editor");

    // ImGui setup
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplGLUT_Init();
    ImGui_ImplOpenGL3_Init("#version 440");

    g_SetupWin = new SetupWindow();
    g_EditViewWin = new EditViewWindow();

    // Terrain shader
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

    gridMesh = terrain::GridGenerator::generate(512, 1.0f);
    gridMesh->setMatrix(glm::mat4(1.0f));
    gridMesh->setMaterial(material);

    // Camera
    glm::mat4 camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 8.0f, 15.0f));
    Eng::PerspectiveCamera* perspectiveCamera = new Eng::PerspectiveCamera("mainCamera", camera);
    perspectiveCamera->setCameraParams(45.0f, RATIO_16_9, 1.0f, 5000.0f);

    // Sun light
    sunLight = new Eng::InfiniteLight("sun");
    updateSunDirection();
    sunLight->setAmbient(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
    sunLight->setDiffuse(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    sunLight->setSpecular(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

    // Scene graph
    Eng::Node* root = eng.getSceneGraphInstance();
    root->addChild(perspectiveCamera);
    root->addChild(sunLight);
    root->addChild(gridMesh);

    mainCamera = perspectiveCamera;
    eng.setActiveCamera(mainCamera);

    // Callbacks
    eng.setOnResizeCallback(onResize);
    eng.setOnSpecialPressedCallback(onSpecialKeyCallback);
    eng.setOnKeyboardPressedCallback(onKeyboardPressedCallback);
    eng.setOnMouseCallback(onMouseCallback);
    eng.setOnMouseMotionCallback(onMouseMotionCallback);
    eng.setOnPassiveMouseMotionCallback(onPassiveMouseMotionCallback);
    eng.setOnTextDrawCallback(renderingImGui);

    eng.start(renderingLoop);

    // Cleanup
    eng.free();

    delete g_SetupWin;
    delete g_EditViewWin;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}