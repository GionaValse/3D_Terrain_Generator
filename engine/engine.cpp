/**
 * @file	engine.cpp
 * @brief	Graphics engine main file
 *
 * @author	Giona Valsecchi (C) SUPSI [giona.valsecchi@supsi.ch] , Pietro Brusadelli (C) SUPSI [pietro.brusadelli@supsi.ch], Filippo De Simoni (C) SUPSI [filippo.desimoni@supsi.ch]
 */

//////////////
// #INCLUDE //
//////////////

// Only for windows user if crash
/*#ifdef _WIN32 or WIN32
#include <Windows.h>
#endif*/

// Main include:
#include "engine.h"

// C/C++:
#include <algorithm>
#include <iostream>
#include <source_location>

// GLM:
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

// Glew (include it before GL.h):
#include <GL/glew.h>

// FreeGLUT:
#include <GL/freeglut.h>

// glutSwapInterval may not be available on older FreeGLUT versions
#ifndef FREEGLUT_VERSION_3_0
#if defined(_MSC_VER)
    extern "C" void glutSwapInterval(int interval);
#else
    extern "C" void glutSwapInterval(int interval) __attribute__((weak));
#endif
#endif

// FreeImage
#define FREEIMAGE_LIB
#include <FreeImage.h>

/////////////
// GLOBALS //
/////////////

bool runningFlag;
int id;
glm::mat4 guiOrtho;

// FPS:
int fps = 0;
int frames = 0;

Eng::Camera* currentActiveCamera{nullptr};

// Callbacks vars
void (*onEngineReshapeCallback)(int width, int height);
void (*onEngineSpecialCallback)(int key, int mouseX, int mouseY);
void (*onEngineKeyboardCallback)(unsigned char key, int mouseX, int mouseY);
void (*onEngineMouseCallback)(int buttonId, int buttonState, int mouseX, int mouseY);
void (*onEngineIdleCallback)();
void (*onEngineCloseCallback)();
void (*onEngineDrawTextCallback)(Eng::GUIObjects gui);

/////////////////
// GL CALLBACK //
/////////////////

static void EngineDisplayCallback() {
    // Nothing to do
}

static void EngineSpecialCallback(int key, int mouseX, int mouseY) {
    if (onEngineSpecialCallback) {
        onEngineSpecialCallback(key, mouseX, mouseY);

        // Force rendering refresh:
        glutPostWindowRedisplay(id);
    }
}

static void EngineKeyboardCallback(unsigned char key, int mouseX, int mouseY) {
    if (onEngineKeyboardCallback) {
        onEngineKeyboardCallback(key, mouseX, mouseY);

        // Force rendering refresh:
        glutPostWindowRedisplay(id);
    }
}

static void EngineMouseCallback(int buttonId, int buttonState, int mouseX, int mouseY) {
    if (onEngineMouseCallback) {
        onEngineMouseCallback(buttonId, buttonState, mouseX, mouseY);

        // Force rendering refresh:
        glutPostWindowRedisplay(id);
    }
}

static void EngineIdleCallback() {
    if (onEngineIdleCallback) {
        onEngineIdleCallback();
    }
}

static void EngineReshapeCallback(int width, int height) {
    if (height == 0)
        height = 1;

    glViewport(0, 0, width, height);
    guiOrtho = glm::ortho(0.0f, (float)width, 0.0f, (float)height, -1.0f, 1.0f);

    glMatrixMode(GL_PROJECTION);

    if (onEngineReshapeCallback) {
        onEngineReshapeCallback(width, height);
    }

    if (currentActiveCamera) {
        Eng::Base::getInstance().fixCameraViewport(currentActiveCamera, width, height);
        //glLoadMatrixf(glm::value_ptr(currentActiveCamera->getProjectionMatrix()));
    }


    glMatrixMode(GL_MODELVIEW);
}

static void EngineCloseCallback() {
    if (onEngineCloseCallback) {
        onEngineCloseCallback();
    }
    runningFlag = false;
}

static void EngineTimerCallback(int value) {
    // Update values:
    fps = frames;
    frames = 0;

    // Register the next update:
    glutTimerFunc(1000, EngineTimerCallback, 0);
}

#ifdef _DEBUG
#ifndef _WIN32
    #define __stdcall // Just defined as an empty macro under Linux
#endif
    /**
     * Debug message callback for OpenGL. See https://www.opengl.org/wiki/Debug_Output
     */
    static void __stdcall EngineDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, GLvoid* userParam)
    {
        std::cout << "OpenGL says: \"" << std::string(message) << "\"" << std::endl;
    }
#endif

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////
// RESERVED STRUCTURES //
/////////////////////////

/**
 * @brief Base class reserved structure (using PIMPL/Bridge design pattern https://en.wikipedia.org/wiki/Opaque_pointer).
 */
struct Eng::Base::Reserved {
    // Flags:
    bool initFlag;

    // Scene components:
    List* sceneList;
    Node* rootNode;

    int width;
    int height;

    /**
     * Constructor.
     */
    Reserved() : initFlag(false),
                 sceneList(new List()),
                 rootNode(nullptr),
                 width(800),
                 height(600) {
    }
};

////////////////////////
// BODY OF CLASS Base //
////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Constructor.
 */
ENG_API Eng::Base::Base() : reserved(std::make_unique<Eng::Base::Reserved>()) {
#ifdef _DEBUG
    std::cout << "[+] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Destructor.
 */
ENG_API Eng::Base::~Base() {
#ifdef _DEBUG
    std::cout << "[-] " << std::source_location::current().function_name() << " invoked" << std::endl;
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Gets a reference to the (unique) singleton instance.
 * @return reference to singleton instance
 */
Eng::Base ENG_API& Eng::Base::getInstance() {
    static Base instance;
    return instance;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Init internal components.
 * @return TF
 */
bool ENG_API Eng::Base::init(int* argc, char* argv[], const char* winName, int width, int height) {
    // Already initialized?
    if (reserved->initFlag) {
        std::cout << "ERROR: engine already initialized" << std::endl;
        return false;
    }

    // INitialize FreeImage
    FreeImage_Initialise();

    // Here you can initialize most of the graphics engine's dependencies and default settings...
    initEngine(argc, argv, winName, width, height);

    // Done:
    std::cout << "[>] " << LIB_NAME << " initialized" << std::endl;
    reserved->initFlag = true;
    return true;
}

/* Private */
void Eng::Base::printInitInfo() {
    // Check OpenGL version:
    std::cout << "OpenGL context" << std::endl;
    std::cout << "   version  . . : " << glGetString(GL_VERSION) << std::endl;
    std::cout << "   vendor . . . : " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "   renderer . . : " << glGetString(GL_RENDERER) << std::endl;

    char* glExtentions = (char*)glGetString(GL_EXTENSIONS);

    /*
    if (strstr(glExtentions, "GL_EXT_bgra")) {
        std::cout << "   GL_EXT_bgra supported!" << std::endl;
    } else if (strstr(glExtentions, "GL_EXT_texture_filter_anisotropic")) {
        std::cout << "   Anisotropic filtering supported" << std::endl;
        // bool isAnisotropicSupported = true;
        int anisotropicLevel = 0;
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &anisotropicLevel);
        std::cout << "   Anisotropic filtering max. level: " << anisotropicLevel << std::endl;
    } else {
        std::cout << "   GL_EXT_bgra NOT supported!" << std::endl;
    }*/
}

void Eng::Base::initEngine(int* argc, char* argv[], const char* winName, int width, int height) {
    glutInit(argc, argv);

    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);

#ifdef _DEBUG
    glutInitContextFlags(GLUT_DEBUG);
#endif

    glutInitWindowSize(width, height);

    glutInitWindowPosition(100, 100);

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

    id = glutCreateWindow(winName);

    // Glew init:
    GLenum glewInitResult = glewInit();

    if (glewInitResult != GLEW_OK) {
        std::cerr << "ERROR: Glew initialization failed: " << glewGetErrorString(glewInitResult) << std::endl;
        return;
    }
    else
    {
        if (GLEW_VERSION_4_4)
            std::cout << "Driver supports OpenGL 4.4\n" << std::endl;
        else
        {
            std::cout << "[ERROR] OpenGL 4.4 not supported\n" << std::endl;
            return;
        }
    }

#ifdef _DEBUG
    glDebugMessageCallback((GLDEBUGPROC)EngineDebugCallback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);
    glEnable(GL_CULL_FACE);

    // light
    glEnable(GL_LIGHTING);
    glLightf(GL_LIGHT_MODEL_LOCAL_VIEWER, 1.0, 1.0);

    glEnable(GL_LIGHT0);
    //  (Qui potresti anche impostare glClearColor, ecc.)

    // 6. IMPOSTAZIONE DEI CALLBACK
    glutDisplayFunc(EngineDisplayCallback);
    glutReshapeFunc(EngineReshapeCallback);
    glutKeyboardFunc(EngineKeyboardCallback);
    glutMouseFunc(EngineMouseCallback);
    glutSpecialFunc(EngineSpecialCallback);
    glutCloseFunc(EngineCloseCallback);
    glutIdleFunc(EngineIdleCallback);

    glutTimerFunc(1000, EngineTimerCallback, 0);

    // Disable v-sync to better measure performance (call this *after* the context creation):
#ifdef _WIN32
    glutSwapInterval(0);
#else
    if (glutSwapInterval)
        glutSwapInterval(0);
#endif

    /* Print engine informations */
    printInitInfo();

    // Initialize shaders (must be after GL context is ready)
    reserved->sceneList->initShaders();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * Free internal components.
 * @return TF
 */
bool ENG_API Eng::Base::free() {
    // Not initialized?
    if (!reserved->initFlag) {
        std::cout << "ERROR: engine not initialized" << std::endl;
        return false;
    }

    // Here you can properly dispose of any allocated resource (including third-party dependencies)...
    FreeImage_DeInitialise();

    // Done:
    std::cout << "[<] " << LIB_NAME << " deinitialized" << std::endl;
    reserved->initFlag = false;
    return true;
}

bool ENG_API Eng::Base::start(void (*callback)(Node* root)) {
    // Not initialized?
    if (!reserved->initFlag) {
        std::cout << "ERROR: engine not initialized" << std::endl;
        return false;
    }

    // Here you can start the main rendering loop...
    // Done:
    std::cout << "[>] engine started" << std::endl;
    runningFlag = true;

    /* Enter the main FreeGLUT processing loop : */
    while (runningFlag) {
        glutMainLoopEvent();

        // Clear buffers:
        glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION);
        if (currentActiveCamera)
            glLoadMatrixf(glm::value_ptr(currentActiveCamera->getProjectionMatrix()));
        glMatrixMode(GL_MODELVIEW);

        // Here you can render the scene...
        callback(reserved->rootNode);

        reserved->sceneList->pass(reserved->rootNode, glm::mat4(1.0f));

        // Render
        reserved->sceneList->render();

        reserved->sceneList->clear();

        if (!runningFlag) {
            break;
        }

        // GUI 2D objects:
        GUIObjects guiObjects;

        guiObjects.start(guiOrtho);
        if (onEngineDrawTextCallback)
            onEngineDrawTextCallback(guiObjects);
        guiObjects.stop();

        frames++;

        // Swap buffers:
        glutSwapBuffers();
    }

    return true;
}

Eng::Node* Eng::Base::getSceneGraphInstance()
{
    if (!reserved->rootNode)
    {
        reserved->rootNode = new Eng::Node("root");
    }

    return reserved->rootNode;
}

ENG_API Eng::Node* Eng::Base::loadScene(std::string path) {
    // Not initialized?
    if (!reserved->initFlag) {
        std::cout << "ERROR: engine not initialized" << std::endl;
        return nullptr;
    }
    // Here you can load a scene from a file...
    // Done:
    std::cout << "[>] scene loaded from: " << path << std::endl;
    OvoReader* reader = new OvoReader();
    reserved->rootNode = reader->load(path);

    if (!reserved->rootNode) {
        std::cerr << "ERROR: Could not load scene from file " << path << std::endl;
        // Clean up:
        delete reader;
        return nullptr;
    }

    delete reader;
    return reserved->rootNode;
}

///////////////////
// Engine Camera //
///////////////////

void Eng::Base::setActiveCamera(Camera* camera) {
    reserved->sceneList->setCamera(camera);
    currentActiveCamera = camera;
    fixCameraViewport(currentActiveCamera, reserved->width, reserved->height);
}

void Eng::Base::fixCameraViewport(Camera* camera, int width, int height) {
    if (!camera)
        return;

    if (Eng::PerspectiveCamera* pCam = dynamic_cast<Eng::PerspectiveCamera*>(currentActiveCamera); pCam != nullptr) {
        float ratio = (float)width / (float)height;
        pCam->setCameraParams(pCam->getFOV(), ratio, pCam->getNearPlane(), pCam->getFarPlane());
    } else if (Eng::OrthoCamera* oCam = dynamic_cast<Eng::OrthoCamera*>(currentActiveCamera); oCam != nullptr) {
        float targetHeight = std::max(1.0f, (-oCam->getBottom()) + oCam->getTop());
        float aspectRatio = (float)width / (float)height;

        float top = targetHeight / 2.0f;
        float bottom = -top;

        float right = top * aspectRatio;
        float left = -right;

        oCam->setCameraParams(left, right, bottom, top, oCam->getNearPlane(), oCam->getFarPlane());
    }

    // Update reserved:
    reserved->width = width;
    reserved->height = height;
}

int Eng::Base::getCurrentFPS() {
    return fps;
}

void Eng::Base::changeWireFrame(bool isWireFrame) {
    if (isWireFrame) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

void Eng::Base::changeVsync(bool isVsync) {
#ifdef _WIN32
    glutSwapInterval(isVsync ? 1 : 0);
#else
    if (glutSwapInterval)
        glutSwapInterval(isVsync ? 1 : 0);
#endif
}



bool Eng::Base::getShadowRender() const {
    return shadowRender;
}

void Eng::Base::setShadowRender(bool shadowRender_) {
    shadowRender = shadowRender_;
}

/////////////////////
// Engine Callback //
/////////////////////

void Eng::Base::setOnResizeCallback(void (*callback)(int width, int height)) {
    onEngineReshapeCallback = callback;
}

void Eng::Base::setOnSpecialPressedCallback(void (*callback)(int key, int mouseX, int mouseY)) {
    onEngineSpecialCallback = callback;
}

void Eng::Base::setOnKeyboardPressedCallback(void (*callback)(unsigned char key, int mouseX, int mouseY)) {
    onEngineKeyboardCallback = callback;
}

void Eng::Base::setOnMouseCallback(void (*callback)(int buttonId, int buttonState, int mouseX, int mouseY)) {
    onEngineMouseCallback = callback;
}

void Eng::Base::setOnIdleCallback(void (*callback)()) {
    onEngineIdleCallback = callback;
}

void Eng::Base::setOnCloseCallback(void (*callback)()) {
    onEngineCloseCallback = callback;
}

void Eng::Base::setOnTextDrawCallback(void (*callback)(GUIObjects gui)) {
    onEngineDrawTextCallback = callback;
}
