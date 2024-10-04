#include "ResourcesManager.h"

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

std::shared_ptr<ShaderProgram> ResourceManager::getShaderProgram(std::string_view shaderName)
{
    ShaderProgramsMap::const_iterator it = m_shaderPrograms.find(shaderName.data());

    if (it != m_shaderPrograms.end())
        return it->second;

    std::cerr << "Can't find the shader program: " << shaderName << std::endl;

    return nullptr;
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

std::string ResourceManager::getFullFilePath(std::string_view relativeFilePath) const
{
    return static_cast<std::string>(m_path + "/" + relativeFilePath.data());
}
