#include "OpenGLManager.h"

#include "LightManager.h"
#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
}

void OpenGLManager::init(GLFWwindow* window)
{
    m_resourceManager->loadShaders("defaultSP", "res/shaders/defaultVert.glsl", "res/shaders/defaultFrag.glsl");
    m_resourceManager->loadShaders("defaultLightSP", "res/shaders/defaultLightVert.glsl", "res/shaders/defaultLightFrag.glsl");

    std::string globalLightFilePath = m_resourceManager->getFullFilePath("res/data/light/globalLight.txt");
    std::string pointLightsFilePath = m_resourceManager->getFullFilePath("res/data/light/pointLights.txt");

    m_lightManager = new LightManager(globalLightFilePath, pointLightsFilePath);

    m_resourceManager->loadNaturalMaterial("res/materials/naturalMaterials.txt");

    std::string cutObjectFilePath = m_resourceManager->getFullFilePath("res/data/object/cutObject.txt");
    m_cutObject = new ReplicatedCutObject(cutObjectFilePath, m_resourceManager, m_lightManager, "emerald");
    m_cutObject->prepareToRenderTrajectory();
    m_cutObject->prepareToRenderTrajectoryCuts();
    m_cutObject->prepareToRenderReplicatedCut();

    m_camera = new Camera(OpenGLConstants::startCameraPosition);

    if (m_mainWindowWidth != 0 && m_mainWindowHeight != 0)
    {
        float aspect = static_cast<float>(m_mainWindowWidth) / m_mainWindowHeight;
        m_projectionMatrix = glm::perspective(OpenGLConstants::fovy, aspect, OpenGLConstants::zNear, OpenGLConstants::zFar);
    }
}

void OpenGLManager::display(GLFWwindow* window, double currentTime)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);
    glEnable(GL_DEPTH_TEST);

    m_viewMatrix = m_camera->getViewMatrix();
    //m_cutObject->renderTrajectory(m_projectionMatrix, m_viewMatrix, m_trajectoryColor);
    //m_cutObject->renderTrajectoryCuts(m_projectionMatrix, m_viewMatrix, m_cutsColor);
    m_cutObject->renderReplicatedCut(m_projectionMatrix, m_viewMatrix, m_replicatedCutColor);
    //m_cutObject->renderNormals(m_projectionMatrix, m_viewMatrix, m_normalsColor);

    glDisable(GL_DEPTH_TEST);
}

void OpenGLManager::windowResize(int width, int height)
{
    m_mainWindowWidth = width;
    m_mainWindowHeight = height;

    if (m_mainWindowWidth != 0 && m_mainWindowHeight != 0)
    {
        float aspect = static_cast<float>(m_mainWindowWidth) / m_mainWindowHeight;
        m_projectionMatrix = glm::perspective(OpenGLConstants::fovy, aspect, OpenGLConstants::zNear, OpenGLConstants::zFar);
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
