#include "GLFWManagement.h"

#include "OpenGLManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <string_view>

namespace GLFWglobals
{
    extern OpenGLManager* openGLManager = nullptr;
    extern GLFWwindow* mainWindow = nullptr;

    extern int mainWindowWidth = 0;
    extern int mainWindowHeight = 0;

    extern float deltaTime = 0.0f;
    extern float lastFrameTime = 0.0f;

    extern bool firstMousePositionChange = true;

    extern float lastMousePositionX = 0.0f;
    extern float lastMousePositionY = 0.0f;
}

namespace GLFW
{
    void init(std::string_view mainWinTitle,
        int mainWinWidth, int mainWinHeight,
        int majorGLVersion, int minorGLVersion,
        std::string_view executablePath)
    {
        GLFWglobals::lastMousePositionX = mainWinWidth / 2.0f;
        GLFWglobals::lastMousePositionY = mainWinHeight / 2.0f;

        GLFWglobals::mainWindowWidth = mainWinWidth;
        GLFWglobals::mainWindowHeight = mainWinHeight;

        if (!glfwInit()) { std::exit(EXIT_FAILURE); }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, majorGLVersion);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minorGLVersion);

        GLFWglobals::mainWindow = glfwCreateWindow(mainWinWidth, mainWinHeight, mainWinTitle.data(), NULL, NULL);

        glfwMakeContextCurrent(GLFWglobals::mainWindow);

        glfwSetCursorPosCallback(GLFWglobals::mainWindow, processCursorPosition);
        glfwSetKeyCallback(GLFWglobals::mainWindow, processKeysClick);
        glfwSetScrollCallback(GLFWglobals::mainWindow, processScroll);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::exit(EXIT_FAILURE); }

        GLFWglobals::openGLManager = new OpenGLManager(executablePath);
        GLFWglobals::openGLManager->init(GLFWglobals::mainWindow);
    }

    void renderLoop()
    {
        while (!glfwWindowShouldClose(GLFWglobals::mainWindow))
        {
            calcDeltaTimePerFrame();
            GLFWglobals::openGLManager->display(GLFWglobals::mainWindow, glfwGetTime());
            glfwSwapBuffers(GLFWglobals::mainWindow);
            glfwPollEvents();
        }
    }

    void destroy()
    {
        if (GLFWglobals::openGLManager) delete GLFWglobals::openGLManager;

        glfwDestroyWindow(GLFWglobals::mainWindow);
        glfwTerminate();
    }

    void processCursorPosition(GLFWwindow* window, double xposIn, double yposIn)
    {
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        {
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (GLFWglobals::firstMousePositionChange)
            {
                GLFWglobals::lastMousePositionX = xpos;
                GLFWglobals::lastMousePositionY = ypos;
                GLFWglobals::firstMousePositionChange = false;
            }

            float xoffset = xpos - GLFWglobals::lastMousePositionX;
            float yoffset = GLFWglobals::lastMousePositionY - ypos; // reversed since y-coordinates go from bottom to top

            GLFWglobals::lastMousePositionX = xpos;
            GLFWglobals::lastMousePositionY = ypos;

            GLFWglobals::openGLManager->mouseMovement(xoffset, yoffset);
        }
    }

    void processKeysClick(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->frontMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->backMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->rightMovement(GLFWglobals::deltaTime);
        }
        else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
        {
            GLFWglobals::openGLManager->leftMovement(GLFWglobals::deltaTime);
        }
    }

    void processScroll(GLFWwindow* window, double xoffset, double yoffset)
    {
        GLFWglobals::openGLManager->mouseScroll(static_cast<float>(yoffset));
    }

    void calcDeltaTimePerFrame()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        GLFWglobals::deltaTime = currentFrame - GLFWglobals::lastFrameTime;
        GLFWglobals::lastFrameTime = currentFrame;
    }
}