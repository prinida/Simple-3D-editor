#ifndef OPENGL_MANAGER_H
#define OPENGL_MANAGER_H

#include "Camera.h"
#include "Enums.h"
#include "LightManager.h"
#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <string_view>
#include <vector>

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

    void setDisplayMode(DisplayModes displayMode);

    const std::vector<std::string>& getNaturalMaterialsNames();
    const std::vector<std::string>& getTexturesNames();

    void setReplicatedCutMaterial(std::string materialName);
    void setReplicatedCutTexture(std::string textureName);

    void addPointLightSource();
    void deletePointLightSource(int index);

    int getPointLightSourceCounts();
    std::string getPointLightSourceName(int index);
    glm::vec3 getAmbientComponent(int index);
    glm::vec3 getDiffuseComponent(int index);
    glm::vec3 getSpecularComponent(int index);

    void setSelectedPointLight(int index);
    void moveSelectedPointLight(int x, int y, int z, double deltaTime);
    void setAmbientComponent(int index, float* ambient);
    void setDiffuseComponent(int index, float* diffuse);
    void setSpecularComponent(int index, float* specular);

    void windowResize(int width, int height);
    void frontMovement(double deltaTime);
    void backMovement(double deltaTime);
    void leftMovement(double deltaTime);
    void rightMovement(double deltaTime);
    void mouseMovement(float xoffset, float yoffset);
    void mouseScroll(float yoffset);

private:
    ResourceManager* m_resourceManager = nullptr;
    ReplicatedCutObject* m_cutObject = nullptr;
    LightManager* m_lightManager = nullptr;

    std::vector<std::string> m_naturalMaterialNames;
    std::vector<std::string> m_texturesNames;

    Camera* m_camera = nullptr;

    glm::mat4 m_projectionMatrix = glm::mat4(1.0f);
    glm::mat4 m_viewMatrix = glm::mat4(1.0f);

    GLuint m_matricesUniformBufferObject{};

    glm::vec3 m_trajectoryColor{ 1.0f, 0.0f, 0.0f };
    glm::vec3 m_cutsColor{ 0.0f, 0.0f, 1.0f };
    glm::vec3 m_replicatedCutColor{ 0.0f, 1.0f, 0.0f };
    glm::vec3 m_normalsColor{ 0.773f, 0.047f, 0.550f };

    int m_mainWindowWidth = 0;
    int m_mainWindowHeight = 0;

    DisplayModes m_displayMode = Replicated_cut_no_smoothing_normals_filled_surface;
};

#endif