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

    ID = glCreateProgram();
    glAttachShader(ID, vertexShaderID);
    glAttachShader(ID, fragmentShaderID);
    glLinkProgram(ID);

    GLint success;
    glGetProgramiv(ID, GL_LINK_STATUS, &success);

    if (!success)
    {
        GLchar infoLog[1024];
        glGetShaderInfoLog(ID, 1024, nullptr, infoLog);
        std::cerr << "ERROR::SHADER: Link time error:\n" << infoLog << std::endl;
    }
    else
        isCompiled = true;

    glDeleteShader(vertexShaderID);
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
    glDeleteProgram(ID);
}

bool ShaderProgram::getIsCompiled() const
{
    return this->isCompiled;
}

void ShaderProgram::use() const
{
    glUseProgram(ID);
}

GLuint ShaderProgram::getID() const
{
    return this->ID;
}

ShaderProgram& ShaderProgram::operator=(ShaderProgram&& shaderProgram) noexcept
{
    glDeleteProgram(ID);
    ID = shaderProgram.ID;
    isCompiled = shaderProgram.isCompiled;

    shaderProgram.ID = 0;
    shaderProgram.isCompiled = false;

    return *this;
}

ShaderProgram::ShaderProgram(ShaderProgram&& shaderProgram) noexcept
{
    ID = shaderProgram.ID;
    isCompiled = shaderProgram.isCompiled;

    shaderProgram.ID = 0;
    shaderProgram.isCompiled = false;
}

// utility uniform functions
 // ------------------------------------------------------------------------
void ShaderProgram::setBool(std::string_view name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.data()), static_cast<int>(value));
}
// ------------------------------------------------------------------------
void ShaderProgram::setInt(std::string_view name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.data()), value);
}
// ------------------------------------------------------------------------
void ShaderProgram::setFloat(std::string_view name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.data()), value);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec2(std::string_view name, const glm::vec2& value) const
{
    glUniform2fv(glGetUniformLocation(ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec2(std::string_view name, float x, float y) const
{
    glUniform2f(glGetUniformLocation(ID, name.data()), x, y);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec3(std::string_view name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec3(std::string_view name, float x, float y, float z) const
{
    glUniform3f(glGetUniformLocation(ID, name.data()), x, y, z);
}
// ------------------------------------------------------------------------
void ShaderProgram::setVec4(std::string_view name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(ID, name.data()), 1, glm::value_ptr(value));
}
void ShaderProgram::setVec4(std::string_view name, float x, float y, float z, float w) const
{
    glUniform4f(glGetUniformLocation(ID, name.data()), x, y, z, w);
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat2(std::string_view name, const glm::mat2& mat) const
{
    glUniformMatrix2fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat3(std::string_view name, const glm::mat3& mat) const
{
    glUniformMatrix3fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}
// ------------------------------------------------------------------------
void ShaderProgram::setMat4(std::string_view name, const glm::mat4& mat) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.data()), 1, GL_FALSE, glm::value_ptr(mat));
}