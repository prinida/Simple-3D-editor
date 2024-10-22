#include "LightManager.h"

#include "LightTypes.h"
#include "ShaderProgram.h"
#include "Sphere.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <fstream>
#include <iostream>

LightManager::LightManager(ResourceManager* resourceManager, std::string_view fullFilePathGlobalLight, std::string_view fullFilePathPointLights)
{
    std::ifstream f;

    f.open(fullFilePathGlobalLight.data());

    if (!f.is_open())
        std::cerr << "Failed to open global light file!" << std::endl;
    else
    {
        glm::vec4* ambient = &m_globalAmbient.ambient;

        f >> ambient->x >> ambient->y >> ambient->z >> ambient->w;

        f.close();
    }

    f.open(fullFilePathPointLights.data());

    if (!f.is_open())
        std::cerr << "Failed to open point lights file!" << std::endl;
    else
    {
        int size = 0;
        f >> size;
        m_pointLight.resize(size);

        glm::vec4* ambient = nullptr;
        glm::vec4* diffuse = nullptr;
        glm::vec4* specular = nullptr;
        glm::vec4* position = nullptr;

        for (int i = 0; i < size; ++i)
        {
            ambient = &m_pointLight[i].ambient;
            diffuse = &m_pointLight[i].diffuse;
            specular = &m_pointLight[i].specular;
            position = &m_pointLight[i].position;

            f >> ambient->x >> ambient->y >> ambient->z >> ambient->w;
            f >> diffuse->x >> diffuse->y >> diffuse->z >> diffuse->w;
            f >> specular->x >> specular->y >> specular->z >> specular->w;
            f >> position->x >> position->y >> position->z;

            position->w = 1.0f;
        }

        f.close();

        glGenBuffers(1, &m_lightsUniformBufferObject);
        glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBufferObject);
        glBufferData(GL_UNIFORM_BUFFER, 1056, nullptr, GL_STATIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 1, m_lightsUniformBufferObject, 0, 1056);

        glBufferSubData(GL_UNIFORM_BUFFER, 0, 16, glm::value_ptr(m_globalAmbient.ambient));
        glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, &size);
        glBufferSubData(GL_UNIFORM_BUFFER, 32, 64 * size, m_pointLight.data());

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }

    Sphere lightSphere{};
    m_elementsSize = lightSphere.GetNumIndices();

    m_defaultShaderProgram = resourceManager->getShaderProgram("defaultSP");

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_lightSphereBufferObject);
    glGenBuffers(1, &m_lightSphereElementBufferObject);

    glBindBuffer(GL_ARRAY_BUFFER, m_lightSphereBufferObject);
    glBufferData(GL_ARRAY_BUFFER, lightSphere.GetNumVertices() * sizeof(float) * 3, lightSphere.GetVertices().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lightSphereElementBufferObject);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, lightSphere.GetNumIndices() * sizeof(int), lightSphere.GetIndices().data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

LightManager::~LightManager()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_lightsUniformBufferObject);
    glDeleteBuffers(1, &m_lightSphereBufferObject);
    glDeleteBuffers(1, &m_lightSphereElementBufferObject);
}

void LightManager::renderPointLights()
{
    m_defaultShaderProgram->use();
    m_defaultShaderProgram->setVec3("color", glm::vec3(1.0f, 1.0f, 1.0f));

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_lightSphereBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_lightSphereElementBufferObject);
    glm::mat4 model(1.0f);

    for (int i = 0; i < m_pointLight.size(); ++i)
    {
        model = glm::translate(glm::mat4(1.0f), glm::vec3(m_pointLight[i].position));
        model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
        m_defaultShaderProgram->setMat4("model_matrix", model);

        glDrawElements(GL_TRIANGLES, m_elementsSize, GL_UNSIGNED_INT, 0);
    }

    if (m_selectedPointLightSource != -1)
    {
        glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
        glStencilMask(0x00);

        m_defaultShaderProgram->setVec3("color", glm::vec3(1.0f, 0.4f, 0.0f));

        model = glm::translate(glm::mat4(1.0f), glm::vec3(m_pointLight[m_selectedPointLightSource].position));
        model = glm::scale(model, glm::vec3(0.23f, 0.23f, 0.23f));
        m_defaultShaderProgram->setMat4("model_matrix", model);

        glDrawElements(GL_TRIANGLES, m_elementsSize, GL_UNSIGNED_INT, 0);

        glStencilMask(0xFF);
        glStencilFunc(GL_ALWAYS, 0, 0xFF);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void LightManager::addPointLightSource()
{
    if (m_pointLight.size() == m_maxPointLightCount)
        return;

    PointLight defaultPointLight;
    defaultPointLight.ambient = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    defaultPointLight.diffuse = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    defaultPointLight.specular = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    defaultPointLight.position = glm::vec4(3.0f, 3.0f, -3.0f, 1.0f);

    m_pointLight.push_back(defaultPointLight);

    updateUniformBufferObject(m_pointLight.size() - 1);
    updateSizeUniformBufferObject();
}

void LightManager::deletePointLightSource(int index)
{
    if (m_pointLight.size() == 0)
        return;

    m_selectedPointLightSource = -1;

    m_pointLight.erase(m_pointLight.begin() + index);

    updateSizeUniformBufferObject();
    updateFullLightPointsInUniformBufferObject();
}

int LightManager::getPointLightSourceCounts()
{
    return m_pointLight.size();
}

std::string LightManager::getPointLightSourceName(int index)
{
    if (index >= m_pointLight.size() || index < 0)
        return std::string();

    return std::format("Point light source {}", index + 1);
}

glm::vec3 LightManager::getAmbientComponent(int index)
{
    if (index >= m_pointLight.size() || index < 0)
        return glm::vec3();

    return glm::vec3(m_pointLight[index].ambient);
}

glm::vec3 LightManager::getDiffuseComponent(int index)
{
    if (index >= m_pointLight.size() || index < 0)
        return glm::vec3();

    return glm::vec3(m_pointLight[index].diffuse);
}

glm::vec3 LightManager::getSpecularComponent(int index)
{
    if (index >= m_pointLight.size() || index < 0)
        return glm::vec3();

    return glm::vec3(m_pointLight[index].specular);
}

void LightManager::setSelectedPointLight(int index)
{
    if (index >= m_pointLight.size())
    {
        m_selectedPointLightSource = -1;
        return;
    }

    m_selectedPointLightSource = index;
}

void LightManager::setSelectedPointLightPosition(int x, int y, int z, double deltaTime)
{
    if (m_selectedPointLightSource != -1)
    {
        int index = m_selectedPointLightSource;

        m_pointLight[index].position.x += x * m_speedCoeff * deltaTime;
        m_pointLight[index].position.y += y * m_speedCoeff * deltaTime;
        m_pointLight[index].position.z += z * m_speedCoeff * deltaTime;

        updateUniformBufferObject(index);
    }
}

void LightManager::setAmbientComponent(int index, glm::vec3 ambient)
{
    if (index >= m_pointLight.size() || index < 0)
        return;

    m_pointLight[index].ambient = glm::vec4(ambient, 1.0f);

    updateUniformBufferObject(index);
}

void LightManager::setDiffuseComponent(int index, glm::vec3 diffuse)
{
    if (index >= m_pointLight.size() || index < 0)
        return;

    m_pointLight[index].diffuse = glm::vec4(diffuse, 1.0f);

    updateUniformBufferObject(index);
}

void LightManager::setSpecularComponent(int index, glm::vec3 specular)
{
    if (index >= m_pointLight.size() || index < 0)
        return;

    m_pointLight[index].specular = glm::vec4(specular, 1.0f);

    updateUniformBufferObject(index);
}

void LightManager::updateUniformBufferObject(int index)
{
    glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 32 + 64 * index, 64, &m_pointLight[index]);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::updateSizeUniformBufferObject()
{
    int size = m_pointLight.size();

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 16, 16, &size);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void LightManager::updateFullLightPointsInUniformBufferObject()
{
    int size = m_pointLight.size();

    glBindBuffer(GL_UNIFORM_BUFFER, m_lightsUniformBufferObject);
    glBufferSubData(GL_UNIFORM_BUFFER, 32, 64 * size, m_pointLight.data());
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
