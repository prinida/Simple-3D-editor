#include "OpenGLManager.h"

#include "ReplicatedCutObject.h"
#include "ResourcesManager.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

OpenGLManager::OpenGLManager(std::string_view executablePath)
{
    m_resourceManager = new ResourceManager(executablePath);
}

OpenGLManager::~OpenGLManager()
{
    if (m_resourceManager) delete m_resourceManager;
    if (m_cutObject) delete m_cutObject;
    if (camera) delete camera;
}

void OpenGLManager::init(GLFWwindow* window)
{
    std::string cutObjectFilePath = m_resourceManager->getFullFilePath("res/data/cutObject.txt");
    m_cutObject = new ReplicatedCutObject(cutObjectFilePath, m_resourceManager);
    m_cutObject->prepareToRenderTrajectory();

    camera = new Camera();
}

void OpenGLManager::display(GLFWwindow* window, double currentTime)
{
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(1, 1, 1, 1);

    viewMatrix = camera->getViewMatrix();
    m_cutObject->renderTrajectory(viewMatrix);
}

void OpenGLManager::frontMovement(double deltaTime)
{
    camera->processKeyboard(FORWARD, deltaTime);
}

void OpenGLManager::backMovement(double deltaTime)
{
    camera->processKeyboard(BACKWARD, deltaTime);
}

void OpenGLManager::leftMovement(double deltaTime)
{
    camera->processKeyboard(LEFT, deltaTime);
}

void OpenGLManager::rightMovement(double deltaTime)
{
    camera->processKeyboard(RIGHT, deltaTime);
}

void OpenGLManager::mouseMovement(float xoffset, float yoffset)
{
    camera->processMouseMovement(xoffset, yoffset);
}

void OpenGLManager::mouseScroll(float yoffset)
{
    camera->processMouseScroll(yoffset);
}
