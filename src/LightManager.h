#ifndef LIGHT_MANAGER_H
#define LIGHT_MANAGER_H

#include "LightTypes.h"
#include "ShaderProgram.h"

#include <memory>
#include <string_view>

class LightManager
{
public:
    LightManager(std::string_view fullFilePathGlobalLight, std::string_view fullFilePathPointLights);

    void setLightUniforms(std::shared_ptr<ShaderProgram> shaderProgram) const;

private:
    GlobalAmbientLight m_globalAmbient{};
    PointLight m_pointLight{};
};

#endif