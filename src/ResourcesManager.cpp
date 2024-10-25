#include "ResourcesManager.h"

#include "MaterialTypes.h"
#include "ShaderProgram.h"
#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <map>
#include <memory>
#include <vector>
#include <filesystem>

ResourceManager::ResourceManager(std::string_view executablePath)
{
    size_t found = executablePath.find_last_of("/\\");
    m_path = executablePath.substr(0, found);
}

std::string ResourceManager::getFileString(std::string_view relativeFilePath) const
{
    std::ifstream f;
    f.open(m_path + "/" + relativeFilePath.data(), std::ios::in | std::ios::binary);

    if (!f.is_open())
    {
        std::cerr << "Failed to open file: " << relativeFilePath << std::endl;
        return std::string();
    }

    std::stringstream buffer;
    buffer << f.rdbuf();

    f.close();

    return buffer.str();
}

std::shared_ptr<ShaderProgram> ResourceManager::loadShaders(std::string_view shaderName, std::string_view vertexPath, std::string_view fragmentPath)
{
    std::string vertexString = getFileString(vertexPath);

    if (vertexString.empty())
    {
        std::cerr << "No vertex shader!" << std::endl;
        return nullptr;
    }

    std::string fragmentString = getFileString(fragmentPath);

    if (fragmentString.empty())
    {
        std::cerr << "No fragment shader!" << std::endl;
        return nullptr;
    }

    auto& newShader = m_shaderPrograms.emplace(shaderName, std::make_shared<ShaderProgram>(vertexString, fragmentString)).first->second;

    if (newShader->getIsCompiled())
        return newShader;

    std::cerr << "Can't load shader program:\n"
        << "Vertex: " << vertexPath << "\n"
        << "Fragment:" << fragmentPath << std::endl;

    return nullptr;
}

std::shared_ptr<ShaderProgram> ResourceManager::loadShaders(std::string_view shaderName, std::string_view vertexPath, std::string_view geomPath, std::string_view fragmentPath)
{
    std::string vertexString = getFileString(vertexPath);

    if (vertexString.empty())
    {
        std::cerr << "No vertex shader!" << std::endl;
        return nullptr;
    }

    std::string geomString = getFileString(geomPath);

    if (geomString.empty())
    {
        std::cerr << "No geometry shader!" << std::endl;
        return nullptr;
    }

    std::string fragmentString = getFileString(fragmentPath);

    if (fragmentString.empty())
    {
        std::cerr << "No fragment shader!" << std::endl;
        return nullptr;
    }

    auto& newShader = m_shaderPrograms.emplace(shaderName, std::make_shared<ShaderProgram>(vertexString, geomString, fragmentString)).first->second;

    if (newShader->getIsCompiled())
        return newShader;

    std::cerr << "Can't load shader program:\n"
        << "Vertex: " << vertexPath << '\n'
        << "Geometry: " << geomPath << '\n'
        << "Fragment:" << fragmentPath << std::endl;

    return nullptr;
}

std::shared_ptr<ShaderProgram> ResourceManager::getShaderProgram(std::string_view shaderName)
{
    ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName.data());

    if (it != m_shaderPrograms.end())
        return it->second;

    std::cerr << "Can't find the shader program: " << shaderName << std::endl;

    return nullptr;
}

void ResourceManager::loadTextures(std::string_view texturesPath)
{
    std::string fullTexturesPath = m_path + "/" + texturesPath.data();
    std::filesystem::directory_iterator texturesDirectory(fullTexturesPath);

    for (auto& file : texturesDirectory)
    {
        auto& filePath = file.path();

        std::string path = texturesPath.data() + filePath.filename().string();
        std::string name = filePath.stem().string();

        loadTexture(name, path);
    }
}

std::shared_ptr<Texture> ResourceManager::loadTexture(std::string_view textureName, std::string_view texturePath)
{
    int nrChannels = 0, width = 0, height = 0;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* pixels = stbi_load(std::string(m_path + "/" + texturePath.data()).c_str(), &width, &height, &nrChannels, STBI_rgb_alpha);

    if (!pixels)
    {
        std::cerr << "Can't load image: " << texturePath << std::endl;

        stbi_image_free(pixels);

        return nullptr;
    }
    else
    {
        auto& newTexture = m_textures.emplace(textureName, std::make_shared<Texture>(pixels, width, height)).first->second;

        stbi_image_free(pixels);

        return newTexture;
    }
}

std::shared_ptr<Texture> ResourceManager::getTexture(std::string_view textureName)
{
    TexturesMap::const_iterator it = m_textures.find(textureName.data());

    if (it != m_textures.end())
        return it->second;

    std::cerr << "Can't find the texture: " << textureName << std::endl;

    return nullptr;
}

std::vector<std::string> ResourceManager::getTexturesNames()
{
    return getNamesFromMap(m_textures);
}

void ResourceManager::loadNaturalMaterial(std::string_view materialPath)
{
    std::ifstream f;

    f.open(m_path + "/" + materialPath.data());

    if (!f.is_open())
        std::cerr << "Failed to open natural materials file!" << std::endl;
    else
    {
        int size = 0;

        f >> size;

        NaturalMaterial material{};

        std::string name;
        glm::vec4* ambient = &material.ambient;
        glm::vec4* diffuse = &material.diffuse;
        glm::vec4* specular = &material.specular;
        float* shininess = &material.shininess;

        for (int i = 0; i < size; ++i)
        {
            f >> name;
            f >> ambient->x >> ambient->y >> ambient->z >> ambient->w;
            f >> diffuse->x >> diffuse->y >> diffuse->z >> diffuse->w;
            f >> specular->x >> specular->y >> specular->z >> specular->w;
            f >> *shininess;

            m_naturalMaterials.emplace(name, std::make_shared<NaturalMaterial>(material));
        }

        f.close();
    }
}

std::shared_ptr<NaturalMaterial> ResourceManager::getNaturalMaterial(std::string_view materialName)
{
    NaturalMaterialsMap::const_iterator it = m_naturalMaterials.find(materialName.data());

    if (it != m_naturalMaterials.end())
        return it->second;

    std::cerr << "Can't find the material: " << materialName << std::endl;

    return nullptr;
}

std::vector<std::string> ResourceManager::getNaturalMaterialNames()
{
    return getNamesFromMap(m_naturalMaterials);
}

std::string ResourceManager::getFullFilePath(std::string_view relativeFilePath) const
{
    return static_cast<std::string>(m_path + "/" + relativeFilePath.data());
}
