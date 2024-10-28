#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "LightTypes.h"
#include "ResourcesManager.h"
#include "ShaderProgram.h"

#include <glad/glad.h>

#include <memory>
#include <string_view>
#include <vector>

class LightManager
{
public:
    LightManager(ResourceManager* resourceManager, std::string_view fullFilePathGlobalLight, std::string_view fullFilePathPointLights);
    ~LightManager();

    void renderPointLights();

    void addPointLightSource();
    void deletePointLightSource(int index);

    void enableGlobalAmbient();
    void disableGlobalAmbient();

    int getPointLightSourceCounts();
    std::string getPointLightSourceName(int index);
    glm::vec3 getAmbientComponent(int index);
    glm::vec3 getDiffuseComponent(int index);
    glm::vec3 getSpecularComponent(int index);

    void setSelectedPointLight(int index);
    void setSelectedPointLightPosition(int x, int y, int z, double deltaTime);
    void setAmbientComponent(int index, glm::vec3 ambient);
    void setDiffuseComponent(int index, glm::vec3 diffuse);
    void setSpecularComponent(int index, glm::vec3 specular);

    void updateUniformBufferObject(int index);
    void updateSizeUniformBufferObject();
    void updateFullLightPointsInUniformBufferObject();

private:
    GlobalAmbientLight m_globalAmbient{};
    std::vector<PointLight> m_pointLight;

    const int m_maxPointLightCount = 16;

    std::shared_ptr<ShaderProgram> m_defaultShaderProgram = nullptr;

    GLuint m_vao{};
    GLuint m_lightSphereBufferObject{};
    GLuint m_lightSphereElementBufferObject{};

    int m_elementsSize = 0;

    int m_selectedPointLightSource = -1;
    float m_speedCoeff = 5.0f;

    GLuint m_lightsUniformBufferObject{};
};

#endif