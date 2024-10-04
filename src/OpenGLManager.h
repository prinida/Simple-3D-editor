#ifndef OPENGL_MANAGER_H
#define OPENGL_MANAGER_H

#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string_view>

class OpenGLManager
{
public:
    OpenGLManager(std::string_view executablePath);
    ~OpenGLManager();

    void init(GLFWwindow* window);
    void display(GLFWwindow* window, double currentTime);

    void frontMovement(double deltaTime);
    void backMovement(double deltaTime);
    void leftMovement(double deltaTime);
    void rightMovement(double deltaTime);
    void mouseMovement(float xoffset, float yoffset);
    void mouseScroll(float yoffset);

private:
    ResourceManager* m_resourceManager = nullptr;
    ReplicatedCutObject* m_cutObject = nullptr;
    Camera* camera = nullptr;

    glm::mat4 viewMatrix = glm::mat4(1.0f);
};

#endif