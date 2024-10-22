#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <string_view>

class ShaderProgram
{
public:
    ShaderProgram(std::string_view vertexShader, std::string_view fragmentShader);
    ShaderProgram(std::string_view vertexShader, std::string_view geomShader, std::string_view fragmentShader);
    ~ShaderProgram();

    ShaderProgram() = delete;
    ShaderProgram(ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;
    ShaderProgram& operator=(ShaderProgram&& ShaderProgram) noexcept;
    ShaderProgram(ShaderProgram&& ShaderProgram) noexcept;

    bool getIsCompiled() const;
    void use() const;
    GLuint getID() const;

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(std::string_view name, bool value) const;
    // ------------------------------------------------------------------------
    void setInt(std::string_view name, int value) const;
    // ------------------------------------------------------------------------
    void setFloat(std::string_view name, float value) const;
    // ------------------------------------------------------------------------
    void setVec2(std::string_view name, const glm::vec2& value) const;
    void setVec2(std::string_view name, float x, float y) const;
    // ------------------------------------------------------------------------
    void setVec3(std::string_view name, const glm::vec3& value) const;
    void setVec3(std::string_view name, float x, float y, float z) const;
    // ------------------------------------------------------------------------
    void setVec4(std::string_view name, const glm::vec4& value) const;
    void setVec4(std::string_view name, float x, float y, float z, float w) const;
    // ------------------------------------------------------------------------
    void setMat2(std::string_view name, const glm::mat2& mat) const;
    // ------------------------------------------------------------------------
    void setMat3(std::string_view name, const glm::mat3& mat) const;
    // ------------------------------------------------------------------------
    void setMat4(std::string_view name, const glm::mat4& mat) const;

private:
    bool createShader(std::string_view source, const GLenum shaderType, GLuint& shaderID) const;

    bool m_isCompiled = false;
    GLuint m_ID = 0;
};

#endif