#include <iostream>
#include <vector>
#include <string>

#include <imgui.h>
#include <backends/imgui_impl_glut.h>
#include <backends/imgui_impl_opengl3.h>

#include "engine.h"

#include "configuration.h"
#include "AppEvents.h"

// Models //
#include "CursorTool.h"
#include "TerrainModel.h"

#include "ErosionBrushTool.h"
#include "SculpingBrushTool.h"
#include "SmoothingBrushTool.h"

#include "ShadesVisualTool.h"
#include "SolidVisualTool.h"
#include "WireframeVisualTool.h"
#include "AltitudeVisualTool.h"

// Views //
#include "TopMenuBar.h"
#include "StatusBar.h"

#include "PointerToolWindow.h"
#include "PointerToolSettingsWindow.h"
#include "VisualToolWindow.h"

#include "SetupWindow.h"

// Controllers //
#include "AppController.h"
#include "CameraGestureController.h"

#include "PointerController.h"
#include "VisualController.h"
#include "SetupController.h"

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

bool isWireFrameMode = false;
bool isGenerated = false;

static int lastMouseX = 0;
static int lastMouseY = 0;
static bool isLeftDragging = false;
static bool isMiddleDragging = false;
static bool isRightDragging = false;

///////////////////////
// Terrain Generator //
///////////////////////

static void generateTerrain()
{
	TerrainModel* terrain = new TerrainModel(
		SetupController::getInstance().getTerrainConfig(),
		SetupController::getInstance().getTextureConfig()
	);

	SetupController::getInstance().setActiveTerrainModel(terrain);
	AppController::getInstance().setTerrainModel(terrain);
	PointerController::getInstance().setTerrainModel(terrain);

	isGenerated = true;
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
	UIController::getInstance().prepareFrame();
}

static void renderingImGui(Eng::GUIObjects obj)
{
	UIController::getInstance().render();
	AppController::getInstance().update();

	if (SetupController::getInstance().consumeGenerationRequest()) {
		generateTerrain();
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
	if (AppController::getInstance().isExportingMesh()) return;

	ImGuiIO& io = ImGui::GetIO();
	addSpecialKeyEvents(io, key, true);
}

static void onSpecialKeyUpCallback(int key, int x, int y)
{
	if (AppController::getInstance().isExportingMesh()) return;

	ImGuiIO& io = ImGui::GetIO();
	addSpecialKeyEvents(io, key, false);
}

static void onKeyboardPressedCallback(unsigned char key, int mouseX, int mouseY)
{
	if (AppController::getInstance().isExportingMesh()) return;
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

}

static void onMouseCallback(int buttonId, int buttonState, int x, int y)
{
	if (AppController::getInstance().isExportingMesh()) return;

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
	if (AppController::getInstance().isExportingMesh()) return;

	ImGui_ImplGLUT_MotionFunc(x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (!isGenerated) return;

	std::string eventType = isLeftDragging ? AppEvents::LEFT_MOUSE_MOVE
		: (isMiddleDragging ? AppEvents::MIDDLE_MOUSE_MOVE
			: (isRightDragging ? AppEvents::RIGHT_MOUSE_MOVE : AppEvents::MOUSE_MOVE));
	MouseMoveDispatcher::getInstance().dispatch(eventType, x, y, lastMouseX, lastMouseY);

	lastMouseX = x;
	lastMouseY = y;
}

static void onPassiveMouseMotionCallback(int x, int y)
{
	if (AppController::getInstance().isExportingMesh()) return;

	ImGui_ImplGLUT_MotionFunc(x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;

	MouseMoveDispatcher::getInstance().dispatch(AppEvents::MOUSE_HOVER, x, y, 0, 0);
}

static void onMouseWheelCallback(int wheelId, int direction, int x, int y)
{
	if (AppController::getInstance().isExportingMesh()) return;

	ImGui_ImplGLUT_MouseWheelFunc(wheelId, direction, x, y);
	if (ImGui::GetIO().WantCaptureMouse) return;
	if (!isGenerated) return;

	MouseWheelDispatcher::getInstance().dispatch(AppEvents::MOUSE_WHEEL, x, y, direction);
}

//////////
// Main //
//////////

int main(int argc, char* argv[])
{
	// Engine Initialization
	Eng::Base& eng = Eng::Base::getInstance();
	eng.init(&argc, argv, "3D Terrain Editor");

	Eng::Camera* mainCamera = nullptr;
	Eng::Shader* terrainShader = nullptr;
	Eng::InfiniteLight* sunLight = nullptr;

	// Shader Setup
	Eng::Shader* vShader = new Eng::Shader();
	Eng::Shader* fShader = new Eng::Shader();
	vShader->loadFromFile(Eng::Shader::TYPE_VERTEX, "./shaders/terrain.vert");
	fShader->loadFromFile(Eng::Shader::TYPE_FRAGMENT, "./shaders/terrain.frag");

	terrainShader = new Eng::Shader();
	terrainShader->build(vShader, fShader);
	terrainShader->render();

	// Visual Tools Setup
	std::vector<std::vector<BaseTool*>> visualToolGroups;
	std::vector<BaseTool*> visualGroup;

	visualGroup.push_back(&ShadesVisualTool::getInstance());
	visualGroup.push_back(&SolidVisualTool::getInstance());
	visualGroup.push_back(&WireframeVisualTool::getInstance());
	visualGroup.push_back(&AltitudeVisualTool::getInstance());

	visualToolGroups.push_back(visualGroup);

	// Brush Tools Setup
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
	PointerToolSettingsWindow* pointerToolSetWin = new PointerToolSettingsWindow();
	VisualToolWindow* visualToolWin = new VisualToolWindow();
	SetupWindow* setupWin = new SetupWindow(
		SetupController::getInstance().getTextureConfig(),
		SetupController::getInstance().getTerrainConfig()
	);

	pointerToolWin->init(brushToolGroups);
	visualToolWin->init(visualToolGroups);

	windows.push_back(pointerToolWin);
	windows.push_back(pointerToolSetWin);
	windows.push_back(visualToolWin);
	windows.push_back(setupWin);

	// Controllers setup
	SetupController& setupController = SetupController::getInstance();
	AppController& appController = AppController::getInstance();
	CameraGestureController& cameraController = CameraGestureController::getInstance();
	PointerController& pointerController = PointerController::getInstance();
	VisualController& visualController = VisualController::getInstance();
	UIController& uiController = UIController::getInstance();

	setupController.init(setupWin);
	appController.init(statusBar);
	uiController.init(windows, topMenuBar, statusBar);
	cameraController.init();
	pointerController.init(pointerToolWin, pointerToolSetWin);
	visualController.init(visualToolWin);

	visualController.setTopMenuSupport(topMenuBar);

	// Camera Setup
	glm::mat4 camera = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 96.0f, 512.0f));
	Eng::PerspectiveCamera* perspectiveCamera = new Eng::PerspectiveCamera("mainCamera", camera);
	perspectiveCamera->setCameraParams(45.0f, RATIO_16_9, 1.0f, 5000.0f);

	// Lighting Setup
	sunLight = new Eng::InfiniteLight("sun");
	sunLight->setDirection(glm::normalize(glm::vec3(-0.5f, -0.707f, -0.5f)));
	sunLight->setAmbient(glm::vec4(0.3f, 0.3f, 0.3f, 1.0f));
	sunLight->setDiffuse(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
	sunLight->setSpecular(glm::vec4(0.5f, 0.5f, 0.5f, 1.0f));

	// Scene Graph Setup
	Eng::Node* root = eng.getSceneGraphInstance();
	root->addChild(perspectiveCamera);
	root->addChild(sunLight);

	mainCamera = perspectiveCamera;
	eng.setActiveCamera(mainCamera);

	// Callbacks Setup
	eng.setOnSpecialPressedCallback(onSpecialKeyDownCallback);
	eng.setOnSpecialReleasedCallback(onSpecialKeyUpCallback);
	eng.setOnKeyboardPressedCallback(onKeyboardPressedCallback);

	eng.setOnMouseCallback(onMouseCallback);
	eng.setOnMouseMotionCallback(onMouseMotionCallback);
	eng.setOnPassiveMouseMotionCallback(onPassiveMouseMotionCallback);
	eng.setOnMouseWheelCallback(onMouseWheelCallback);

	eng.setOnResizeCallback(onResize);
	eng.setOnTextDrawCallback(renderingImGui);

	// Start Main Loop
	eng.start(renderingLoop);

	// Cleanup
	eng.free();

	appController.free();
	cameraController.free();
	pointerController.free();
	visualController.free();
	SetupController::getInstance().free();

	uiController.free();

	return 0;
}
