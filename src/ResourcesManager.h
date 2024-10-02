#ifndef RESOURCES_MANAGER_H
#define RESOURCES_MANAGER_H

#include "ShaderProgram.h"
#include "Texture.h"

#include <map>
#include <memory>
#include <string>
#include <string_view>

class ResourceManager
{
public:
    ResourceManager(std::string_view executablePath);
    ~ResourceManager() = default;

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
    ResourceManager& operator=(ResourceManager&&) = delete;
    ResourceManager(ResourceManager&&) = delete;

    std::shared_ptr<ShaderProgram> LoadShaders(std::string_view shaderName, std::string_view vertexPath, std::string_view fragmentPath);
    std::shared_ptr<ShaderProgram> GetShaderProgram(std::string_view shaderName);

    std::shared_ptr<Texture> LoadTexture(std::string_view textureName, std::string_view texturePath);
    std::shared_ptr<Texture> GetTexture(std::string_view textureName);

private:
    std::string GetFileString(std::string_view relativeFilePath) const;

    typedef std::map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgramsMap;
    ShaderProgramsMap shaderPrograms;

    typedef std::map<std::string, std::shared_ptr<Texture>> TexturesMap;
    TexturesMap textures;

    std::string path;
};

#endif
