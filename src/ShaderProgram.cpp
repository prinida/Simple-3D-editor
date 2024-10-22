#include "ShaderProgram.h"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <string>
#include <string_view>

ShaderProgram::ShaderProgram(std::string_view vertexShader, std::string_view fragmentShader)
{
    GLuint vertexShaderID, fragmentShaderID;

    if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID))
    {
        std::cerr << "VERTEX::SHADER: Compile time error:\n" << std::endl;
        return;
    }

    if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID))
    {
        std::cerr << "FRAGMENT::SHADER: Compile time error:\n" << std::endl;
        glDeleteShader(vertexShaderID);
        return;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShaderID);
    glAttachShader(m_ID, fragmentShaderID);
    glLinkProgram(m_ID);

    GLint success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(m_ID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER: Link time error:\n" << infoLog << std::endl;
    }
    else
        m_isCompiled = true;

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);
}

ShaderProgram::ShaderProgram(std::string_view vertexShader, std::string_view geomShader, std::string_view fragmentShader)
{
    GLuint vertexShaderID, geomShaderID, fragmentShaderID;

    if (!createShader(vertexShader, GL_VERTEX_SHADER, vertexShaderID))
    {
        std::cerr << "VERTEX::SHADER: Compile time error:\n" << std::endl;
        return;
    }

    if (!createShader(geomShader, GL_GEOMETRY_SHADER, geomShaderID))
    {
        std::cerr << "GEOMETRY::SHADER: Compile time error:\n" << std::endl;
        return;
    }

    if (!createShader(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderID))
    {
        std::cerr << "FRAGMENT::SHADER: Compile time error:\n" << std::endl;
        glDeleteShader(vertexShaderID);
        return;
    }

    m_ID = glCreateProgram();
    glAttachShader(m_ID, vertexShaderID);
    glAttachShader(m_ID, geomShaderID);
    glAttachShader(m_ID, fragmentShaderID);
    glLinkProgram(m_ID);

    GLint success;
    glGetProgramiv(m_ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(m_ID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER: Link time error:\n" << infoLog << std::endl;
    }
    else
        m_isCompiled = true;

    glDeleteShader(vertexShaderID);
    glDeleteShader(geomShaderID);
    glDeleteShader(fragmentShaderID);
}

bool ShaderProgram::createShader(std::string_view source, const GLenum shaderType, GLuint& shaderID) const
{
    shaderID = glCreateShader(shaderType);
    const char* code = source.data();
    glShaderSource(shaderID, 1, &code, nullptr);
    glCompileShader(shaderID);

    GLint success;
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(shaderID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER: Compile time error:\n" << infoLog << std::endl;
        return false;
    }

    return true;
}

ShaderProgram::~ShaderProgram()
{
    glDeleteProgram(m_ID);
}

bool ShaderProgram::getIsCompiled() const
{
    return this->m_isCompiled;
}

void ShaderProgram::use() const
{
    glUseProgram(m_ID);
}

GLuint ShaderProgram::getID() const
{
    return this->m_ID;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) noexcept
{
    glDeleteProgram(m_ID);
    m_ID = shaderProgram.m_ID;
    m_isCompiled = shaderProgram.m_isCompiled;

    shaderProgram.m_ID = 0;
    shaderProgram.m_isCompiled = false;

    return *this;
}

ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept
{
    m_ID = shaderProgram.m_ID;
    m_isCompiled = shaderProgram.m_isCompiled;

    shaderProgram.m_ID = 0;
    shaderProgram.m_isCompiled = false;
}

// utility uniform functions
 // ------------------------------------------------------------------------
void ShaderProgram::setBool(std::string_view name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.data()), static_cast<int>(value));
}
// ------------------------------------------------------------------------
void ShaderProgram::setInt(std::string_view name, int value) const
{
    glUniform1i(glGetUniformLocation(m_ID, name.data()), value);
}
// ------------------------------------------------------------------------
void ShaderProgram::setFloat(std::string_view name, float value) const
{
    glUniform1f(glGetUniformLocation(m_ID, name.data()), value);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec2(std::string_view name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(m_ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec2(std::string_view name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(m_ID, name.data()), x, y);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec3(std::string_view name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec3(std::string_view name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(m_ID, name.data()), x, y, z);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec4(std::string_view name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec4(std::string_view name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(m_ID, name.data()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat2(std::string_view name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(m_ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat3(std::string_view name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(m_ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat4(std::string_view name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}