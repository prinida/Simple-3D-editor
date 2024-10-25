#include "OpenGLManager.h"

#include "LightManager.h"
#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

OpenGLManager::OpenGLManager(std::string_view executablePath, int mainWindowWidth, int mainWindowHeight) :
    m_mainWindowWidth(mainWindowWidth),
    m_mainWindowHeight(mainWindowHeight)
{
    m_resourceManager = new ResourceManager(executablePath);
}

OpenGLManager::~OpenGLManager()
{
    if (m_resourceManager) delete m_resourceManager;
    if (m_lightManager) delete m_lightManager;
    if (m_cutObject) delete m_cutObject;
    if (m_camera) delete m_camera;

    glDeleteBuffers(1, &m_matricesUniformBufferObject);
}

void OpenGLManager::init(GLFWwindow* window)
{
    m_resourceManager->loadShaders(
        "defaultSP", 
        "res/shaders/defaultVert.glsl", 
        "res/shaders/defaultFrag.glsl");
    m_resourceManager->loadShaders(
        "defaultTextureSP",
        "res/shaders/defaultTextureVert.glsl",
        "res/shaders/defaultTextureFrag.glsl");
    m_resourceManager->loadShaders(
        "defaultLightSP", 
        "res/shaders/defaultLightVert.glsl", 
        "res/shaders/defaultLightFrag.glsl");
    m_resourceManager->loadShaders(
        "defaultNormalsSP",
        "res/shaders/defaultNormalsVert.glsl",
        "res/shaders/defaultNormalsGeom.glsl",
        "res/shaders/defaultFrag.glsl");

    std::string globalLightFilePath = m_resourceManager->getFullFilePath("res/data/light/globalLight.txt");
    std::string pointLightsFilePath = m_resourceManager->getFullFilePath("res/data/light/pointLights.txt");

    m_lightManager = new LightManager(m_resourceManager, globalLightFilePath, pointLightsFilePath);

    m_resourceManager->loadNaturalMaterial("res/materials/naturalMaterials.txt");
    m_naturalMaterialNames = m_resourceManager->getNaturalMaterialNames();

    m_resourceManager->loadTextures("res/textures/");
    m_texturesNames = m_resourceManager->getTexturesNames();

    std::string cutObjectFilePath = m_resourceManager->getFullFilePath("res/data/object/cutObject.txt");
    m_cutObject = new ReplicatedCutObject(cutObjectFilePath, m_resourceManager, m_naturalMaterialNames[0], m_texturesNames[0]);
    m_cutObject->prepareToRenderTrajectory();
    m_cutObject->prepareToRenderTrajectoryCuts();
    m_cutObject->prepareToRenderReplicatedCut();

    m_camera = new Camera(OpenGLConstants::startCameraPosition);

    if (m_mainWindowWidth != 0 && m_mainWindowHeight != 0)
    {
        float aspect = static_cast<float>(m_mainWindowWidth) / m_mainWindowHeight;
        m_projectionMatrix = glm::perspective(OpenGLConstants::fovy, aspect, OpenGLConstants::zNear, OpenGLConstants::zFar);
    }

    glGenBuffers(1, &m_matricesUniformBufferObject);
    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniformBufferObject);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_matricesUniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projectionMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OpenGLManager::display(GLFWwindow* window, double currentTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    m_viewMatrix = m_camera->getViewMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(m_viewMatrix));
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    glStencilMask(0x00);

    switch (m_displayMode)
    {
    case Trajectory:
        m_cutObject->renderTrajectory(m_trajectoryColor);
        break;

    case Trajectory_and_filled_cuts:
        m_cutObject->renderTrajectory(m_trajectoryColor);
        m_cutObject->renderTrajectoryCuts(m_cutsColor, false);
        break;

    case Trajectory_and_frame_cuts:
        m_cutObject->renderTrajectory(m_trajectoryColor);
        m_cutObject->renderTrajectoryCuts(m_cutsColor, true);
        break;

    case Replicated_cut_smoothing_normals_filled_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, false, true, false, true);
        break;

    case Replicated_cut_no_smoothing_normals_filled_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, false, true, false, false);
        break;

    case Replicated_cut_smoothing_normals_display_filled_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, false, true, true, true);
        break;

    case Replicated_cut_no_smoothing_normals_display_filled_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, false, true, true, false);
        break;

    case Replicated_cut_no_light_filled_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, false, false, false, false);
        break;

    case Replicated_cut_simple_frame_surface:
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor,  true, false, false, false);
        break;

    case Replicated_cut_trajectory_frame_surface:
        m_cutObject->renderTrajectory(m_trajectoryColor);
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, true, false, false, false);
        break;

    case Replicated_cut_trajectory_and_cuts_frame_surface:
        m_cutObject->renderTrajectory(m_trajectoryColor);
        m_cutObject->renderTrajectoryCuts(m_cutsColor, true);
        m_cutObject->renderReplicatedCut(m_replicatedCutColor, m_normalsColor, true, false, false, false);
        break;
    }

    m_lightManager->renderPointLights();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_STENCIL_TEST);
}

void OpenGLManager::setDisplayMode(DisplayModes displayMode)
{
    m_displayMode = displayMode;
}

const std::vector<std::string>& OpenGLManager::getNaturalMaterialsNames()
{
    return m_naturalMaterialNames;
}

const std::vector<std::string>& OpenGLManager::getTexturesNames()
{
    return m_texturesNames;
}

void OpenGLManager::setReplicatedCutMaterial(std::string materialName)
{
    m_cutObject->setMaterial(materialName);
}

void OpenGLManager::setReplicatedCutTexture(std::string textureName)
{
    m_cutObject->setTexture(textureName);
}

void OpenGLManager::addPointLightSource()
{
    m_lightManager->addPointLightSource();
}

void OpenGLManager::deletePointLightSource(int index)
{
    m_lightManager->deletePointLightSource(index);
}

int OpenGLManager::getPointLightSourceCounts()
{
    return m_lightManager->getPointLightSourceCounts();
}

std::string OpenGLManager::getPointLightSourceName(int index)
{
    return m_lightManager->getPointLightSourceName(index);
}

glm::vec3 OpenGLManager::getAmbientComponent(int index)
{
    return m_lightManager->getAmbientComponent(index);
}

glm::vec3 OpenGLManager::getDiffuseComponent(int index)
{
    return m_lightManager->getDiffuseComponent(index);
}

glm::vec3 OpenGLManager::getSpecularComponent(int index)
{
    return m_lightManager->getSpecularComponent(index);
}

void OpenGLManager::setSelectedPointLight(int index)
{
    m_lightManager->setSelectedPointLight(index);
}

void OpenGLManager::moveSelectedPointLight(int x, int y, int z, double deltaTime)
{
    m_lightManager->setSelectedPointLightPosition(x, y, z, deltaTime);
}

void OpenGLManager::setAmbientComponent(int index, float* ambient)
{
    m_lightManager->setAmbientComponent(index, glm::vec3(ambient[0], ambient[1], ambient[2]));
}

void OpenGLManager::setDiffuseComponent(int index, float* diffuse)
{
    m_lightManager->setDiffuseComponent(index, glm::vec3(diffuse[0], diffuse[1], diffuse[2]));
}

void OpenGLManager::setSpecularComponent(int index, float* specular)
{
    m_lightManager->setSpecularComponent(index, glm::vec3(specular[0], specular[1], specular[2]));
}

void OpenGLManager::windowResize(int width, int height)
{
    m_mainWindowWidth = width;
    m_mainWindowHeight = height;

    if (m_mainWindowWidth != 0 && m_mainWindowHeight != 0)
    {
        float aspect = static_cast<float>(m_mainWindowWidth) / m_mainWindowHeight;
        m_projectionMatrix = glm::perspective(OpenGLConstants::fovy, aspect, OpenGLConstants::zNear, OpenGLConstants::zFar);

        glBindBuffer(GL_UNIFORM_BUFFER, m_matricesUniformBufferObject);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projectionMatrix));
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
}

void OpenGLManager::frontMovement(double deltaTime)
{
    m_camera->processKeyboard(FORWARD, deltaTime);
}

void OpenGLManager::backMovement(double deltaTime)
{
    m_camera->processKeyboard(BACKWARD, deltaTime);
}

void OpenGLManager::leftMovement(double deltaTime)
{
    m_camera->processKeyboard(LEFT, deltaTime);
}

void OpenGLManager::rightMovement(double deltaTime)
{
    m_camera->processKeyboard(RIGHT, deltaTime);
}

void OpenGLManager::mouseMovement(float xoffset, float yoffset)
{
    m_camera->processMouseMovement(xoffset, yoffset);
}

void OpenGLManager::mouseScroll(float yoffset)
{
    m_camera->processMouseScroll(yoffset);
}
