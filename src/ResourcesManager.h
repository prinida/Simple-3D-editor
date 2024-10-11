#ifndef RESOURCES_MANAGER_H
#define RESOURCES_MANAGER_H

#include "MaterialTypes.h"
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

    std::shared_ptr<ShaderProgram> loadShaders(std::string_view shaderName, std::string_view vertexPath, std::string_view fragmentPath);
    std::shared_ptr<ShaderProgram> getShaderProgram(std::string_view shaderName);

    std::shared_ptr<Texture> loadTexture(std::string_view textureName, std::string_view texturePath);
    std::shared_ptr<Texture> getTexture(std::string_view textureName);

    void loadNaturalMaterial(std::string_view materialPath);
    std::shared_ptr<NaturalMaterial> getNaturalMaterial(std::string_view materialName);

    std::string getFullFilePath(std::string_view relativeFilePath) const;

private:
    std::string getFileString(std::string_view relativeFilePath) const;

    typedef std::map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgramsMap;
    ShaderProgramsMap m_shaderPrograms;

    typedef std::map<std::string, std::shared_ptr<Texture>> TexturesMap;
    TexturesMap m_textures;

    typedef std::map<std::string, std::shared_ptr<NaturalMaterial>> NaturalMaterialsMap;
    NaturalMaterialsMap m_naturalMaterials;

    std::string m_path;
};

#endif
