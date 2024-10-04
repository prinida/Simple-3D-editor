#include "ReplicatedCutObject.h"

#include "ResourcesManager.h"

#include <glad/glad.h>

#include <fstream>
#include <iostream>
#include <string_view>

ReplicatedCutObject::ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager)
{
    m_resourceManager = resourceManager;

    std::ifstream f;
    f.open(fullFilePath.data(), std::ios::in);

    if (!f.is_open())
        std::cerr << "Failed to open file cut object file!" << std::endl;
    else
    {
        generateBuffers();

        int cutPointAmount = 0;
        f >> cutPointAmount;

        m_cut.resize(cutPointAmount);

        float x = 0, y = 0, z = 0;

        for (int i = 0; i < cutPointAmount; ++i)
        {
            f >> x >> y;
            m_cut[i] = glm::vec2(x, y);
        }

        int trajectoryPointAmount = 0;
        f >> trajectoryPointAmount;

        m_trajectory.resize(trajectoryPointAmount);

        for (int i = 0; i < trajectoryPointAmount; ++i)
        {
            f >> x >> y >> z;
            m_trajectory[i] = glm::vec3(x, y, z);
        }

        int cutParametersAmount = 0;
        f >> cutParametersAmount;

        m_cutParameters.resize(cutParametersAmount);

        for (int i = 0; i < cutParametersAmount; ++i)
            f >> m_cutParameters[i];

        f.close();
    }
}

ReplicatedCutObject::~ReplicatedCutObject()
{
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_trajectoryBufferObject);
}

void ReplicatedCutObject::prepareToRenderTrajectory()
{
    m_trajectoryShaderProgram = m_resourceManager->loadShaders(
        "trajectoryLineSP",
        "res/shaders/trajectoryLineVert.glsl",
        "res/shaders/trajectoryLineFrag.glsl");

    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_trajectory.size() * sizeof(float) * 3, m_trajectory.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReplicatedCutObject::renderTrajectory(const glm::mat4& viewMatrix)
{
    m_trajectoryShaderProgram->use();
    m_trajectoryShaderProgram->setMat4("view_matrix", viewMatrix);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINE_STRIP, 0, m_trajectory.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
}

void ReplicatedCutObject::generateBuffers()
{
    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_trajectoryBufferObject);
}
