#ifndef GLFW_MANAGEMENT_H
#define GLFW_MANAGEMENT_H

#include "OpenGLManager.h"

#include <GLFW/glfw3.h>

#include <string_view>

namespace GLFWglobals
{
    extern OpenGLManager* openGLManager;
    extern GLFWwindow* mainWindow;

    extern int mainWindowWidth;
    extern int mainWindowHeight;
    
    extern float deltaTime;
    extern float lastFrameTime;

    extern bool firstMousePositionChange;

    extern float lastMousePositionX;
    extern float lastMousePositionY;
}

namespace GLFW
{
    void init(std::string_view mainWinTitle,
        int mainWinWidth, int mainWinHeight,
        int majorGLVersion, int minorGLVersion,
        std::string_view executablePath);
    void renderLoop();
    void destroy();

    void renderMenu();

    void processCursorPosition(GLFWwindow* window, double xposIn, double yposIn);
    void processKeysClick(GLFWwindow* window, int key, int scancode, int action, int mods);
    void processScroll(GLFWwindow* window, double xoffset, double yoffset);
    void processMouseClick(GLFWwindow* window, int button, int action, int mods);
    void processWindowResize(GLFWwindow* window, int width, int height);
    void calcDeltaTimePerFrame();
}

#endif