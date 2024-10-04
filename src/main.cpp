#include "GLFWManagement.h"

#include <string>

int main(int argc, char** argv)
{
    std::string mainWinTitle = "Simple 3D editor";
    int mainWinWidth = 1280, mainWinHeight = 720;
    int majorGLVersion = 4, minorGLVersion = 6;
    std::string executablePath = argv[0];

    GLFW::init(mainWinTitle, mainWinWidth, mainWinHeight, majorGLVersion, minorGLVersion, executablePath);
    GLFW::renderLoop();
    GLFW::destroy();
}