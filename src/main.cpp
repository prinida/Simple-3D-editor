#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdlib>

int main(int argc, char** argv)
{
    if (!glfwInit()) { std::exit(EXIT_FAILURE); }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(600, 600, "Simple-3D-editor", NULL, NULL);

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { std::exit(EXIT_FAILURE); }

    while (!glfwWindowShouldClose(window))
    {
        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    std::exit(EXIT_SUCCESS);
}