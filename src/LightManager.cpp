#include "LightManager.h"

#include "ShaderProgram.h"

#include <glm/glm.hpp>

#include <fstream>
#include <iostream>

LightManager::LightManager(std::string_view fullFilePathGlobalLight, std::string_view fullFilePathPointLights)
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
        glm::vec4* ambient = &m_pointLight.ambient;
        glm::vec4* diffuse = &m_pointLight.diffuse;
        glm::vec4* specular = &m_pointLight.specular;
        glm::vec3* position = &m_pointLight.position;

        f >> ambient->x >> ambient->y >> ambient->z >> ambient->w;
        f >> diffuse->x >> diffuse->y >> diffuse->z >> diffuse->w;
        f >> specular->x >> specular->y >> specular->z >> specular->w;
        f >> position->x >> position->y >> position->z;

        f.close();
    }
}

void LightManager::setLightUniforms(std::shared_ptr<ShaderProgram> shaderProgram) const
{
    shaderProgram->use();

    shaderProgram->setVec4("globalAmbient", m_globalAmbient.ambient);

    shaderProgram->setVec4("light.ambient", m_pointLight.ambient);
    shaderProgram->setVec4("light.diffuse", m_pointLight.diffuse);
    shaderProgram->setVec4("light.specular", m_pointLight.specular);
    shaderProgram->setVec3("light.position", m_pointLight.position);
}
