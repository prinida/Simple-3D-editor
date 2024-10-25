#ifndef RESOURCES_MANAGER_H
#define RESOURCES_MANAGER_H

#include "MaterialTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include <map>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

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
    std::shared_ptr<ShaderProgram> loadShaders(std::string_view shaderName, std::string_view vertexPath, std::string_view geomPath, std::string_view fragmentPath);
    std::shared_ptr<ShaderProgram> getShaderProgram(std::string_view shaderName);

    void loadTextures(std::string_view texturesPath);
    std::shared_ptr<Texture> loadTexture(std::string_view textureName, std::string_view texturePath);
    std::shared_ptr<Texture> getTexture(std::string_view textureName);
    std::vector<std::string> getTexturesNames();

    void loadNaturalMaterial(std::string_view materialPath);
    std::shared_ptr<NaturalMaterial> getNaturalMaterial(std::string_view materialName);
    std::vector<std::string> getNaturalMaterialNames();

    std::string getFullFilePath(std::string_view relativeFilePath) const;

private:
    std::string getFileString(std::string_view relativeFilePath) const;

    template<typename T>
    std::vector<std::string> getNamesFromMap(const std::map<std::string, T>& mapContainer) const;

    typedef std::map<std::string, std::shared_ptr<ShaderProgram>> ShaderProgramsMap;
    ShaderProgramsMap m_shaderPrograms;

    typedef std::map<std::string, std::shared_ptr<Texture>> TexturesMap;
    TexturesMap m_textures;

    typedef std::map<std::string, std::shared_ptr<NaturalMaterial>> NaturalMaterialsMap;
    NaturalMaterialsMap m_naturalMaterials;

    std::string m_path;
};

template<typename T>
std::vector<std::string> ResourceManager::getNamesFromMap(const std::map<std::string, T>& mapContainer) const
{
    int size = mapContainer.size();
    std::vector<std::string> names(size);

    int i = 0;
    for (auto& var : mapContainer)
    {
        names[i] = var.first;
        ++i;
    }

    return names;
}

#endif
