#ifndef OPENGL_MANAGER_H
#define OPENGL_MANAGER_H

#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"
#include "Camera.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string_view>

namespace OpenGLConstants
{
    inline constexpr float fovy = 1.0472f;
    inline constexpr float zNear = 0.1f;
    inline constexpr float zFar = 300.0f;

    inline constexpr glm::vec3 startCameraPosition(0.0f, 0.0f, 1.0f);
}

class OpenGLManager
{
public:
    OpenGLManager(std::string_view executablePath, int mainWindowWidth, int mainWindowHeight);
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
    Camera* m_camera = nullptr;

    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);

    glm::vec3 m_trajectoryColor{ 1.0f, 0.0f, 0.0f };
    glm::vec3 m_cutsColor{ 0.0f, 0.0f, 1.0f };
    glm::vec3 m_replicatedCutColor{ 0.0f, 1.0f, 0.0f };

    int m_mainWindowWidth = 0;
    int m_mainWindowHeight = 0;
};

#endif