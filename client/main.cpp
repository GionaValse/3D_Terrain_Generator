#include <iostream>
#include <vector>
#include <string>

#include <atomic>
#include <future>

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

#include "TerrainGenerator.hpp"
#include "ImageExporter.hpp"
#include "ObjExporter.hpp"
#include "GridGenerator.hpp"

#include "SetupWindow.h"
#include "LoadingWindow.h"
#include "ToolWindow.h"

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
LoadingWindow* g_LoadingWin = nullptr;
ToolWindow* g_ToolWin = nullptr;

std::vector<float> image;
bool isGenerated = false;

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

static int lastMouseX = 0;
static int lastMouseY = 0;
static bool isLeftDragging = false;
static bool isMiddleDragging = false;
static bool isRightDragging = false;

/////////////
// ATOMICS //
/////////////

std::atomic<bool> isExporting = false;

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

    if (heightMap)
    {
        delete heightMap;
        heightMap = nullptr;
    }

    heightMap = new Eng::Texture("TerrainHeightMap", config.size, config.size, image);
    isGenerated = true;
}

static void exportTerrain()
{
    isExporting = true;

    terrain::TerrainConfig config = g_SetupWin->getTerrainConfiguartion();
    float heightScale = g_SetupWin->getHeightScale();

    std::thread([
        imgData = image,
        size = config.size,
        hScale = heightScale,
        &mesh = *gridMesh
    ]() mutable {
            ObjExporter::exportToObj("./bin/export/terrain.obj", mesh, imgData, size, hScale);
            isExporting = false;
        }).detach();
}

static void findCoordOnTexture(glm::vec3 coords)
{
    terrain::TerrainConfig config = g_SetupWin->getTerrainConfiguartion();
    float terrainPhysicalSize = 512.0f;
    int imageResolution = config.size;

    float localX = coords.x + (terrainPhysicalSize / 2.0f);
    float localZ = coords.z + (terrainPhysicalSize / 2.0f);

    float u = localX / terrainPhysicalSize;
    float v = localZ / terrainPhysicalSize;

    int pixelX = static_cast<int>(u * imageResolution);
    int pixelY = static_cast<int>(v * imageResolution);

    if (pixelX < 0 || pixelX >= imageResolution || pixelY < 0 || pixelY >= imageResolution)
        return;

    int brushRadius = 15;
    float brushStrength = 0.005f;

    int startX = std::max(0, pixelX - brushRadius);
    int startY = std::max(0, pixelY - brushRadius);
    int endX = std::min(imageResolution - 1, pixelX + brushRadius);
    int endY = std::min(imageResolution - 1, pixelY + brushRadius);

    bool modified = false;

    for (int y = startY; y <= endY; ++y) 
    {
        for (int x = startX; x <= endX; ++x) 
        {
            float distX = (float)(x - pixelX);
            float distY = (float)(y - pixelY);
            float distance = std::sqrt(distX * distX + distY * distY);

            if (distance <= brushRadius)
            {
                float falloff = 1.0f - (distance / brushRadius);
                float raiseAmount = brushStrength * falloff;
                int index = (y * imageResolution + x) * 3;

                image[index + 0] = std::clamp(image[index + 0] + raiseAmount, 0.0f, 1.0f);
                image[index + 1] = std::clamp(image[index + 1] + raiseAmount, 0.0f, 1.0f);
                image[index + 2] = std::clamp(image[index + 2] + raiseAmount, 0.0f, 1.0f);

                modified = true;
            }
        }
    }

    if (modified && heightMap != nullptr) 
    {
        int updateWidth = endX - startX + 1;
        int updateHeight = endY - startY + 1;

        heightMap->updateSubImage(startX, startY, updateWidth, updateHeight, image, imageResolution);
    }
}

//////////////////
// WINDOW VIEWS //
//////////////////

static void renderMainMenuBar()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Nuovo", "Ctrl+N"))
        {
            isGenerated = false;
        }
        if (ImGui::MenuItem("Salva EXR", "Ctrl+S")) { /* Logica */ }
        if (ImGui::MenuItem("Esporta terreno", "Ctrl+E"))
        {
            exportTerrain();
        }
        ImGui::Separator();
        if (ImGui::MenuItem("Esci", "Alt+F4"))
        {
            Eng::Base::getInstance().stop();
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("Modifica"))
    {
        if (ImGui::MenuItem("Reset Camera")) { /* Logica */ }

#ifdef _DEBUG
        if (ImGui::MenuItem(isWireFrameMode ? "Realistico" : "Wireframe"))
        { 
            isWireFrameMode = !isWireFrameMode;
            Eng::Base::getInstance().changeWireFrame(isWireFrameMode);
        }
#endif // _DEBUG

        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

static void renderStatusBar()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();

    float barHeight = 25.0f;
    ImGui::SetNextWindowPos(ImVec2(viewport->Pos.x, viewport->Pos.y + viewport->Size.y - barHeight));
    ImGui::SetNextWindowSize(ImVec2(viewport->Size.x, barHeight));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollWithMouse |
        ImGuiWindowFlags_NoSavedSettings;

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(5, 2));
    if (ImGui::Begin("StatusBar", nullptr, flags))
    {
        int currentFps = Eng::Base::getInstance().getCurrentFPS();
        ImGui::Text("FPS: %d | Status: Ready | Camera Pos: %.1f, %.1f, %.1f",
            currentFps,
            mainCamera->getMatrix()[3].x,
            mainCamera->getMatrix()[3].y,
            mainCamera->getMatrix()[3].z);

        ImGui::End();
    }
    ImGui::PopStyleVar();
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
    renderMainMenuBar();
    renderStatusBar();

    if (!isGenerated && g_SetupWin) g_SetupWin->render();
    if (isExporting && g_LoadingWin) g_LoadingWin->render();
    if (g_ToolWin) g_ToolWin->render();

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

static void addSpecialKeyEvents(ImGuiIO& io, int key, bool down)
{
    switch (key)
    {
    case ENGINE_KEY_F1:     io.AddKeyEvent(ImGuiKey_F1, true); break;
    case ENGINE_KEY_F2:     io.AddKeyEvent(ImGuiKey_F2, true); break;
    case ENGINE_KEY_LEFT:   io.AddKeyEvent(ImGuiKey_LeftArrow, true); break;
    case ENGINE_KEY_RIGHT:  io.AddKeyEvent(ImGuiKey_RightArrow, true); break;
    case ENGINE_KEY_UP:     io.AddKeyEvent(ImGuiKey_UpArrow, true); break;
    case ENGINE_KEY_DOWN:   io.AddKeyEvent(ImGuiKey_DownArrow, true); break;
    case ENGINE_KEY_PAGE_UP:   io.AddKeyEvent(ImGuiKey_PageUp, true); break;
    case ENGINE_KEY_PAGE_DOWN: io.AddKeyEvent(ImGuiKey_PageDown, true); break;
    case ENGINE_KEY_HOME:   io.AddKeyEvent(ImGuiKey_Home, true); break;
    case ENGINE_KEY_END:    io.AddKeyEvent(ImGuiKey_End, true); break;
    case ENGINE_KEY_INSERT: io.AddKeyEvent(ImGuiKey_Insert, true); break;
    case ENGINE_KEY_DELETE: io.AddKeyEvent(ImGuiKey_Delete, true); break;

    case ENGINE_KEY_CTRL_L: io.AddKeyEvent(ImGuiKey_LeftCtrl, true); break;
    case ENGINE_KEY_CTRL_R:  io.AddKeyEvent(ImGuiKey_RightCtrl, true); break;
    case ENGINE_KEY_SHIFT_L: io.AddKeyEvent(ImGuiKey_LeftShift, true); break;
    case ENGINE_KEY_SHIFT_R: io.AddKeyEvent(ImGuiKey_RightShift, true); break;
    case ENGINE_KEY_ALT_L: io.AddKeyEvent(ImGuiKey_LeftAlt, true); break;
    case ENGINE_KEY_ALT_R: io.AddKeyEvent(ImGuiKey_RightAlt, true); break;
    }
}

static void onSpecialKeyDownCallback(int key, int x, int y)
{
    if (isExporting) return;

    ImGuiIO& io = ImGui::GetIO();
    addSpecialKeyEvents(io, key, true);
    // if (io.WantCaptureKeyboard) return; // If callback needed uncomment this line
}

static void onSpecialKeyUpCallback(int key, int x, int y)
{
    if (isExporting) return;

    ImGuiIO& io = ImGui::GetIO();
    addSpecialKeyEvents(io, key, false);
    // if (io.WantCaptureKeyboard) return; // If callback needed uncomment this line
}

static void onKeyboardPressedCallback(unsigned char key, int mouseX, int mouseY)
{
    if (isExporting) return;
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

    if (!isGenerated) return;

#ifdef _DEBUG
    if (key == 'v')
    {
        isWireFrameMode = !isWireFrameMode;
        Eng::Base::getInstance().changeWireFrame(isWireFrameMode);
    }
#endif

    const float rotSpeed = 2.0f;
    const float moveSpeed = 20.0f;

    // Toggle sun rotation
    if (key == 'g')
    {
        sunRotating = !sunRotating;
    }
}

static void onMouseCallback(int buttonId, int buttonState, int x, int y)
{
    if (isExporting) return;

    ImGui_ImplGLUT_MouseFunc(buttonId, buttonState, x, y);
    if (ImGui::GetIO().WantCaptureMouse) return;
    if (!isGenerated) return;

    if (buttonId == ENGINE_MOUSE_BUTTON_LEFT) 
    {
        isLeftDragging = (buttonState == ENGINE_MOUSE_BUTTON_DOWN);
    } 
    else if (buttonId == ENGINE_MOUSE_BUTTON_MIDDLE) 
    {
        isMiddleDragging = (buttonState == ENGINE_MOUSE_BUTTON_DOWN);
    }
    else if (buttonId == ENGINE_MOUSE_BUTTON_RIGHT) 
    {
        isRightDragging = (buttonState == ENGINE_MOUSE_BUTTON_DOWN);
    }

    lastMouseX = x;
    lastMouseY = y;
}

static void onMouseMotionCallback(int x, int y) 
{
    if (isExporting) return;

    ImGui_ImplGLUT_MotionFunc(x, y);
    if (ImGui::GetIO().WantCaptureMouse) return;
    if (!isGenerated) return;

    float deltaX = (float)(x - lastMouseX);
    float deltaY = (float)(y - lastMouseY);

    float mouseSensitivity = 0.2f;

    if (isLeftDragging) {
        glm::vec3 clickedPos;

        if (Eng::Base::getInstance().getClickedNode(x, y, clickedPos))
            findCoordOnTexture(clickedPos);
    }

    if (isMiddleDragging) {
        glm::mat4 matrix = mainCamera->getMatrix();
        glm::vec3 localRight = glm::normalize(glm::vec3(matrix[0]));

        glm::mat4 rotationX = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaY * mouseSensitivity), localRight);
        glm::mat4 rotationY = glm::rotate(glm::mat4(1.0f), glm::radians(-deltaX * mouseSensitivity), glm::vec3(0.0f, 1.0f, 0.0f));

        mainCamera->setMatrix(rotationY * rotationX * matrix);
    }

    if (isRightDragging) {
        float panSpeed = 0.5f;

        glm::vec3 right = glm::normalize(glm::vec3(mainCamera->getMatrix()[0]));
        glm::vec3 up = glm::normalize(glm::vec3(mainCamera->getMatrix()[1]));

        glm::vec3 translation = (right * (-deltaX * panSpeed)) + (up * (deltaY * panSpeed));

        mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), translation) * mainCamera->getMatrix());
    }

    lastMouseX = x;
    lastMouseY = y;
}

static void onPassiveMouseMotionCallback(int x, int y) 
{
    if (isExporting) return;

    ImGui_ImplGLUT_MotionFunc(x, y);
    // if (ImGui::GetIO().WantCaptureMouse) return; // If callback needed uncomment this line
}

static void onMouseWheelCallback(int wheelId, int direction, int x, int y)
{
    if (isExporting) return;

    ImGui_ImplGLUT_MouseWheelFunc(wheelId, direction, x, y);
    ImGuiIO& io = ImGui::GetIO();
    if (io.WantCaptureMouse) return;
    if (!isGenerated) return;

    bool isControlPressed = io.KeyCtrl;

    float sensitivity = isControlPressed ? 5.0f : 10.0f;
    float scrollDelta = (direction > 0) ? sensitivity : -sensitivity;

    glm::vec3 forward = -glm::vec3(mainCamera->getMatrix()[2]);

    glm::vec3 translation = glm::normalize(forward) * scrollDelta;
    mainCamera->setMatrix(glm::translate(glm::mat4(1.0f), translation) * mainCamera->getMatrix());
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
    g_LoadingWin = new LoadingWindow();
    g_ToolWin = new ToolWindow();

    // Terrain shader
    Eng::Shader* vShader = new Eng::Shader();
    Eng::Shader* fShader = new Eng::Shader();
    vShader->loadFromFile(Eng::Shader::TYPE_VERTEX, "./shaders/terrain.vert");
    fShader->loadFromFile(Eng::Shader::TYPE_FRAGMENT, "./shaders/terrain.frag");

    terrainShader = new Eng::Shader();
    terrainShader->build(vShader, fShader);
    terrainShader->render();

    Eng::Material* material = new Eng::Material("TerrainMaterial");
    material->setSpecular(glm::vec4(0.0f));
    material->setShader(terrainShader);
    material->setTexture(heightMap);

    gridMesh = terrain::GridGenerator::generate(512, 1.0f);
    gridMesh->setMatrix(glm::mat4(1.0f));
    gridMesh->setMaterial(material);

    // Camera
    glm::mat4 camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 96.0f, 512.0f));
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
    eng.setOnSpecialPressedCallback(onSpecialKeyDownCallback);
    eng.setOnSpecialReleasedCallback(onSpecialKeyUpCallback);
    eng.setOnKeyboardPressedCallback(onKeyboardPressedCallback);

    eng.setOnMouseCallback(onMouseCallback);
    eng.setOnMouseMotionCallback(onMouseMotionCallback);
    eng.setOnPassiveMouseMotionCallback(onPassiveMouseMotionCallback);
    eng.setOnMouseWheelCallback(onMouseWheelCallback);

    eng.setOnResizeCallback(onResize);
    eng.setOnTextDrawCallback(renderingImGui);
    eng.start(renderingLoop);

    // Cleanup
    eng.free();

    delete g_SetupWin;
    delete g_LoadingWin;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGLUT_Shutdown();
    ImGui::DestroyContext();

    return 0;
}