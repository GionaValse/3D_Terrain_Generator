#include <iostream>
#include <vector>
#include <string>

#include <atomic>
#include <future>

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

#include "TerrainGenerator.h"
#include "TextureGenerator.h"
#include "ImageExporter.hpp"
#include "ObjExporter.hpp"

#include "configuration.h"

// Models //
#include "CursorTool.h"

#include "ErosionBrushTool.h"
#include "SculpingBrushTool.h"
#include "SmoothingBrushTool.h"

#include "ShadesVisualTool.h"
#include "SolidVisualTool.h"
#include "WireframeVisualTool.h"

// Views //
#include "TopMenuBar.h"
#include "StatusBar.h"

#include "PointerToolWindow.h"
#include "VisualToolWindow.h"

#include "SetupWindow.h"
#include "LoadingWindow.h"

// Controllers //
#include "AppController.h"
#include "CameraGestureController.h"
#include "ConfigController.h"

#include "PointerController.h"
#include "VisualController.h"

#include "UIController.h"

/////////////////
// DISPATCHERS //
/////////////////

using MouseMoveDispatcher = EventDispatcher<int, int, int, int>;
using MouseWheelDispatcher = EventDispatcher<int, int, int>;

/////////////
// Globals //
/////////////

std::vector<BaseWindow*> windows;

Eng::Camera* mainCamera = nullptr;
Eng::Mesh* gridMesh = nullptr;
Eng::Shader* terrainShader = nullptr;
Eng::InfiniteLight* sunLight = nullptr;

SetupWindow* g_SetupWin = nullptr;
LoadingWindow* g_LoadingWin = nullptr;

std::vector<float> image;
bool isWireFrameMode = false;
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

static void generateTerrain(float heightScale)
{
	ConfigController& config = ConfigController::getInstance();

	TextureConfig textureConfiguration = config.getActiveTextureConfig();
	TerrainConfig terrainConfiguration = config.getActiveTerrainConfig();

	TextureGenerator textureGenerator(textureConfiguration);
	TerrainGenerator terrainGenerator(terrainConfiguration);

	image.clear();
	image = textureGenerator.generate();
	std::string textureName;

	if (!terrain::ImageExporter::saveEXR(image, textureConfiguration, textureName))
	{
		std::cerr << "Operazione fallita.\n";
		return;
	}

	std::cout << "\nOperazione completata.\n";
	std::cout << "\n--- TEST GENERAZIONE GRIGLIA ---\n";

	terrainShader->setFloat(terrainShader->getParamLocation("heightScale"), heightScale);
	config.setHeightMapImage(image);

	Eng::Node* root = Eng::Base::getInstance().getSceneGraphInstance();

	if (gridMesh)
	{
		root->removeChild(gridMesh);
		delete gridMesh;
		gridMesh = nullptr;
	}

	Eng::Texture* heightMap = new Eng::Texture("TerrainHeightMap", textureConfiguration.size, textureConfiguration.size, image);
	isGenerated = true;

	Eng::Material* material = new Eng::Material("TerrainMaterial");
	material->setSpecular(glm::vec4(0.0f));
	material->setTexture(heightMap);

	gridMesh = terrainGenerator.generate();
	gridMesh->setMatrix(glm::mat4(1.0f));
	gridMesh->setMaterial(material);

	root->addChild(gridMesh);

	AppController::getInstance().setTerrain(image, gridMesh);
	PointerController::getInstance().setHeightMap(heightMap);
}

static void exportTerrain()
{
	isExporting = true;


	TerrainConfig terrainConfiguration = ConfigController::getInstance().getActiveTerrainConfig();
	float heightScale = g_SetupWin->getHeightScale();

	std::thread([
		imgData = image,
		size = terrainConfiguration.size,
		hScale = heightScale,
		&mesh = *gridMesh
	]() mutable {
			ObjExporter::exportToObj("./bin/export/terrain.obj", mesh, imgData, size, hScale);
			isExporting = false;
		}).detach();
}

///////////////
// Callbacks //
///////////////

void onResize(int w, int h) {
	ImGui_ImplGLUT_ReshapeFunc(w, h);
	UIController::getInstance().onResize(w, h);
}

static void renderingLoop(Eng::Node* root)
{
	// Auto-rotate sun
	if (sunRotating)
	{
		sunAzimuth += glm::radians(0.5f);
		updateSunDirection();
	}

	UIController::getInstance().prepareFrame();
}

static void renderingImGui(Eng::GUIObjects obj)
{
	UIController::getInstance().render();
	AppController::getInstance().update();

	if (!isGenerated && g_SetupWin) g_SetupWin->render();
	if (isExporting && g_LoadingWin) g_LoadingWin->render();

	if (g_SetupWin && g_SetupWin->checkAndResetTrigger()) {
		generateTerrain( g_SetupWin->getHeightScale());
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
	TerrainConfig& terrainConfig = ConfigController::getInstance().getActiveTerrainConfig();
	if (isExporting) return;

	ImGui_ImplGLUT_MotionFunc(x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (!isGenerated) return;

	std::string eventType = isLeftDragging ? "LEFT_MOUSE_MOVE" 
		: (isMiddleDragging ? "MIDDLE_MOUSE_MOVE" 
			: (isRightDragging ? "RIGHT_MOUSE_MOVE" : "MOUSE_MOVE"));
	MouseMoveDispatcher::getInstance().dispatch(eventType, x, y, lastMouseX, lastMouseY);

	lastMouseX = x;
	lastMouseY = y;
}

static void onPassiveMouseMotionCallback(int x, int y)
{
	if (isExporting) return;

	ImGui_ImplGLUT_MotionFunc(x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;

	MouseMoveDispatcher::getInstance().dispatch("MOUSE_HOVER", x, y, 0, 0);
}

static void onMouseWheelCallback(int wheelId, int direction, int x, int y)
{
	if (isExporting) return;

	ImGui_ImplGLUT_MouseWheelFunc(wheelId, direction, x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (!isGenerated) return;

	MouseWheelDispatcher::getInstance().dispatch("MOUSE_WHEEL", x, y, direction);
}

//////////
// Main //
//////////

int main(int argc, char* argv[])
{
	Eng::Base& eng = Eng::Base::getInstance();
	eng.init(&argc, argv, "3D Terrain Editor");

	// Terrain shader
	Eng::Shader* vShader = new Eng::Shader();
	Eng::Shader* fShader = new Eng::Shader();
	vShader->loadFromFile(Eng::Shader::TYPE_VERTEX, "./shaders/terrain.vert");
	fShader->loadFromFile(Eng::Shader::TYPE_FRAGMENT, "./shaders/terrain.frag");

	terrainShader = new Eng::Shader();
	terrainShader->build(vShader, fShader);
	terrainShader->render();	

	g_SetupWin = new SetupWindow();
	g_LoadingWin = new LoadingWindow();

	// Visual tools setup
	std::vector<std::vector<BaseTool*>> visualToolGroups;
	std::vector<BaseTool*> visualGroup;

	visualGroup.push_back(&ShadesVisualTool::getInstance());
	visualGroup.push_back(&SolidVisualTool::getInstance());
	visualGroup.push_back(&WireframeVisualTool::getInstance());

	visualToolGroups.push_back(visualGroup);

	// Brush tools setup
	std::vector<std::vector<BaseTool*>> brushToolGroups;
	std::vector<BaseTool*> selectGroup;
	std::vector<BaseTool*> brushGroup;

	selectGroup.push_back(new CursorTool(CURSOR_SENSITIVITY));

	brushGroup.push_back(&SculptingBrushTool::getInstance());
	brushGroup.push_back(&SmoothingBrushTool::getInstance());
	brushGroup.push_back(&ErosionBrushTool::getInstance());

	brushToolGroups.push_back(selectGroup);
	brushToolGroups.push_back(brushGroup);

	// Views setup
	TopMenuBar* topMenuBar = new TopMenuBar();
	StatusBar* statusBar = new StatusBar();

	PointerToolWindow* pointerToolWin = new PointerToolWindow();
	VisualToolWindow* visualToolWin = new VisualToolWindow();

	pointerToolWin->init(brushToolGroups);
	visualToolWin->init(visualToolGroups);

	windows.push_back(pointerToolWin);
	windows.push_back(visualToolWin);

	// Controllers setup
	AppController& appController = AppController::getInstance();
	CameraGestureController& cameraController = CameraGestureController::getInstance();
	PointerController& pointerController = PointerController::getInstance();
	VisualController& visualController = VisualController::getInstance();
	UIController& uiController = UIController::getInstance();

	appController.init(topMenuBar, statusBar);
	uiController.init(windows, topMenuBar, statusBar);
	cameraController.init();
	pointerController.init(pointerToolWin);
	visualController.init(visualToolWin);

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

	appController.free();
	cameraController.free();
	pointerController.free();
	visualController.free();

	delete g_SetupWin;
	delete g_LoadingWin;

	uiController.free();

	return 0;
}