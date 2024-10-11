#include "ReplicatedCutObject.h"

#include "ResourcesManager.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <iostream>
#include <string_view>

ReplicatedCutObject::ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager, LightManager* lightManager, std::string_view material)
{
    m_resourceManager = resourceManager;
    m_lightManager = lightManager;
    m_defaultShaderProgram = m_resourceManager->getShaderProgram("defaultSP");
    m_defaultLightShaderProgram = m_resourceManager->getShaderProgram("defaultLightSP");
    m_material = resourceManager->getNaturalMaterial(material);

    std::ifstream f;
    f.open(fullFilePath.data(), std::ios::in);

    if (!f.is_open())
        std::cerr << "Failed to open cut object file!" << std::endl;
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
    glDeleteBuffers(1, &m_trajectoryCutsBufferObject);
    glDeleteBuffers(1, &m_replicatedCutBufferObject);
    glDeleteBuffers(1, &m_replicatedCutNormalsBufferObject);
    glDeleteBuffers(1, &m_normalsBufferObject);
}

void ReplicatedCutObject::prepareToRenderTrajectory()
{
    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_trajectory.size() * sizeof(float) * 3, m_trajectory.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReplicatedCutObject::renderTrajectory(const glm::vec3& color)
{
    glEnable(GL_LINE_SMOOTH);

    m_defaultShaderProgram->use();
    m_defaultShaderProgram->setVec3("color", color);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINE_STRIP, 0, m_trajectory.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDisable(GL_LINE_SMOOTH);
}

void ReplicatedCutObject::prepareToRenderTrajectoryCuts()
{
    // сделать каскадный вызов функций?
    // сделать обработку когда коллиниарны линии, когда траектория идет вдоль оси z
    // сделать обработку когда передана одна точка, две точки
    // посмотреть что будет если задавать точки траектории в разных плоскостях
    // посмотреть что будет если крутить траекторию в разные плоскости (не только z = 0)
    // что делать, когда сечения пересекаются из за своих размеров и маленького расстояния между точками траектории

    // учесть то, что будет если product будет нулем
    calcVectorsOrientationInTrajectory();

    int cutSize = m_cut.size();
    int trajectorySize = m_trajectory.size();

    glm::vec3 cutCenter{};
    for (int i = 0; i < cutSize; ++i)
        cutCenter += glm::vec3(m_cut[i], 0.0f);
    cutCenter /= cutSize;

    // тут этого не должно быть, сделано временно (сохранять оригинальное сечение)
    glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.3f, 0.3f, 0.3f));
    glm::mat4 tran = glm::translate(glm::mat4(1.0f), -cutCenter);
    for (int i = 0; i < cutSize; ++i)
    {
        m_cut[i] = glm::vec2(scale * tran * glm::vec4(m_cut[i], 0.0f, 1.0f));
    }

    m_translatedCut.resize((cutSize + 2) * trajectorySize);

    glm::vec3 y{};

    for (int i = 0, shift = 1; i < trajectorySize && trajectorySize >= 3; ++i, shift += cutSize + 2)
    {
        glm::vec3 p1{}, p2{}, p3{};
        glm::vec3 center{};
        glm::vec3 translate{};
        glm::vec3 x{}, z{};

        if (i == 0)
        {
            p1 = m_trajectory[0];
            p2 = m_trajectory[1];
            p3 = m_trajectory[2];

            glm::vec3 a = p2 - p1;
            glm::vec3 b = p3 - p2;

            z = glm::normalize(a);
            y = glm::normalize(glm::cross(a, b));
            x = glm::normalize(glm::cross(z, y));

            glm::vec3 nextX = glm::normalize(-a) + glm::normalize(b);
            if (m_isChangeVectorOrientation[i + 1]) nextX = -nextX;

            float angle1 = glm::acos(glm::dot(x, nextX));
            float angle2 = glm::acos(glm::dot(-x, nextX));

            if (angle2 < angle1) x = -x;

            center = p1;
            translate = p1;
        }
        else if (i == m_trajectory.size() - 1)
        {
            p1 = m_trajectory[i - 2];
            p2 = m_trajectory[i - 1];
            p3 = m_trajectory[i];

            glm::vec3 a = p2 - p1;
            glm::vec3 b = p3 - p2;

            z = glm::normalize(b);
            x = glm::normalize(glm::cross(z, y));

            glm::vec3 prevX = glm::normalize(-a) + glm::normalize(b);
            if (m_isChangeVectorOrientation[i - 1]) prevX = -prevX;

            float angle1 = glm::acos(glm::dot(x, prevX));
            float angle2 = glm::acos(glm::dot(-x, prevX));

            if (angle2 < angle1) x = -x;

            center = p3;
            translate = p3 - m_trajectory[0];
        }
        else
        {
            p1 = m_trajectory[i - 1];
            p2 = m_trajectory[i];
            p3 = m_trajectory[i + 1];

            glm::vec3 a = glm::normalize(p1 - p2);
            glm::vec3 b = glm::normalize(p3 - p2);

            x = glm::normalize(a + b);
            z = glm::normalize(glm::cross(x, y));

            if (m_isChangeVectorOrientation[i]) x = -x;

            center = p2;
            translate = p2 - m_trajectory[0];
        }

        glm::mat3 rotate = glm::mat3(x, y, z);

        for (int j = 0; j < cutSize; ++j)
            m_translatedCut[j + shift] = rotate * glm::vec3(m_cut[j], 0.0f) + translate;

        m_translatedCut[i * (cutSize + 2)] = center;
        m_translatedCut[shift + cutSize] = m_translatedCut[shift];
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryCutsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_translatedCut.size() * sizeof(float) * 3, m_translatedCut.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReplicatedCutObject::renderTrajectoryCuts(const glm::vec3& color, bool isFrameMode)
{
    m_defaultShaderProgram->use();
    m_defaultShaderProgram->setVec3("color", color);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_trajectoryCutsBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (isFrameMode)
    {
        glEnable(GL_POLYGON_MODE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    int cutSize = m_cut.size();
    for (int i = 0; i < m_trajectory.size(); ++i)
        glDrawArrays(GL_TRIANGLE_FAN, i * (cutSize + 2), cutSize + 2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    if (isFrameMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_MODE);
    }
}

void ReplicatedCutObject::prepareToRenderReplicatedCut()
{
    int cutSize = m_cut.size();
    int pointsInCutNum = cutSize + 2;
    int cutNum = m_trajectory.size();

    int replicatedCutSize = (cutNum - 1) * cutSize * 2 * 3 + cutSize * 3 * 2;
    int normalsSize = (cutNum - 1) * cutSize * 2 + 4;

    m_replicatedCut.resize(replicatedCutSize);
    m_replicatedCutNormals.resize(replicatedCutSize);
    m_normals.resize(normalsSize);

    int repCutIndex = 0;
    int normalsIndex = 0;

    for (int i = 0; i < cutNum - 1; ++i)
    {
        for (int j = 1; j < pointsInCutNum - 1; ++j)
        {
            int currentCutIndex = i * pointsInCutNum + j;
            int nextCutIndex = (i + 1) * pointsInCutNum + j;

            glm::vec3 point1 = m_translatedCut[currentCutIndex];
            glm::vec3 point2 = m_translatedCut[currentCutIndex + 1];
            glm::vec3 point3 = m_translatedCut[nextCutIndex];
            glm::vec3 point4 = m_translatedCut[nextCutIndex + 1];

            m_replicatedCut[repCutIndex] = point1;
            m_replicatedCut[repCutIndex + 1] = point2;
            m_replicatedCut[repCutIndex + 2] = point3;
            m_replicatedCut[repCutIndex + 3] = point2;
            m_replicatedCut[repCutIndex + 4] = point3;
            m_replicatedCut[repCutIndex + 5] = point4;

            glm::vec3 a = point1 - point2;
            glm::vec3 b = point3 - point2;

            glm::vec3 normal = glm::cross(a, b);

            m_replicatedCutNormals[repCutIndex] = normal;
            m_replicatedCutNormals[repCutIndex + 1] = normal;
            m_replicatedCutNormals[repCutIndex + 2] = normal;
            m_replicatedCutNormals[repCutIndex + 3] = normal;
            m_replicatedCutNormals[repCutIndex + 4] = normal;
            m_replicatedCutNormals[repCutIndex + 5] = normal;

            repCutIndex += 6;

            glm::vec3 normalStart = (point2 + point3) / 2.0f;
            glm::vec3 normalEnd = normalStart + 0.1f * glm::normalize(normal);
            m_normals[normalsIndex] = normalStart;
            m_normals[normalsIndex + 1] = normalEnd;

            normalsIndex += 2;
        }
    }

    glm::vec3 center = m_translatedCut[0];
    glm::vec3 normal = m_trajectory[0] - m_trajectory[1];

    for (int i = 1; i < cutSize + 1; ++i)
    {
        m_replicatedCut[repCutIndex] = center;
        m_replicatedCut[repCutIndex + 1] = m_translatedCut[i];
        m_replicatedCut[repCutIndex + 2] = m_translatedCut[i + 1];

        m_replicatedCutNormals[repCutIndex] = normal;
        m_replicatedCutNormals[repCutIndex + 1] = normal;
        m_replicatedCutNormals[repCutIndex + 2] = normal;

        repCutIndex += 3;
    }

    m_normals[normalsIndex] = m_trajectory[0];
    m_normals[normalsIndex + 1] = m_trajectory[0] + 0.1f * glm::normalize(normal);
    normalsIndex += 2;

    center = m_translatedCut[m_translatedCut.size() - cutSize - 2];
    normal = m_trajectory[m_trajectory.size() - 1] - m_trajectory[m_trajectory.size() - 2];

    for (int i = 1; i < cutSize + 1; ++i)
    {
        m_replicatedCut[repCutIndex] = center;
        m_replicatedCut[repCutIndex + 1] = m_translatedCut[m_translatedCut.size() - i];
        m_replicatedCut[repCutIndex + 2] = m_translatedCut[m_translatedCut.size() - i - 1];

        m_replicatedCutNormals[repCutIndex] = normal;
        m_replicatedCutNormals[repCutIndex + 1] = normal;
        m_replicatedCutNormals[repCutIndex + 2] = normal;

        repCutIndex += 3;
    }

    m_normals[normalsIndex] = m_trajectory[m_trajectory.size() - 1];
    m_normals[normalsIndex + 1] = m_trajectory[m_trajectory.size() - 1] + 0.1f * glm::normalize(normal);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_replicatedCut.size() * sizeof(float) * 3, m_replicatedCut.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutNormalsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_replicatedCutNormals.size() * sizeof(float) * 3, m_replicatedCutNormals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_normals.size() * sizeof(float) * 3, m_normals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReplicatedCutObject::renderReplicatedCut(const glm::vec3& color, bool isFrameMode, bool isLightEnabled)
{
    if (isLightEnabled)
    {
        m_lightManager->setLightUniforms(m_defaultLightShaderProgram);

        m_defaultLightShaderProgram->use();

        // это должно быть где-то снаружи!
        m_defaultLightShaderProgram->setVec4("material.ambient", m_material->ambient);
        m_defaultLightShaderProgram->setVec4("material.diffuse", m_material->diffuse);
        m_defaultLightShaderProgram->setVec4("material.specular", m_material->specular);
        m_defaultLightShaderProgram->setFloat("material.shininess", m_material->shininess);
    }
    else
    {
        m_defaultShaderProgram->use();

        m_defaultShaderProgram->setVec3("color", color);
    }

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutNormalsBufferObject);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    if (isFrameMode)
    {
        glEnable(GL_POLYGON_MODE);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    glDrawArrays(GL_TRIANGLES, 0, m_replicatedCut.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    if (isFrameMode)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glDisable(GL_POLYGON_MODE);
    }
}

void ReplicatedCutObject::renderNormals(const glm::vec3& color, bool isSmoothMode)
{
    m_defaultShaderProgram->use();

    m_defaultShaderProgram->setVec3("color", color);

    glEnable(GL_LINE_SMOOTH);

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_normalsBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glDrawArrays(GL_LINES, 0, m_normals.size());

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);

    glDisable(GL_LINE_SMOOTH);
}

void ReplicatedCutObject::generateBuffers()
{
    glCreateVertexArrays(1, &m_vao);
    glCreateBuffers(1, &m_trajectoryBufferObject);
    glCreateBuffers(1, &m_trajectoryCutsBufferObject);
    glCreateBuffers(1, &m_replicatedCutBufferObject);
    glCreateBuffers(1, &m_replicatedCutNormalsBufferObject);
    glCreateBuffers(1, &m_normalsBufferObject);
}

void ReplicatedCutObject::calcVectorsOrientationInTrajectory()
{
    int trajectorySize = m_trajectory.size();
    m_isChangeVectorOrientation.resize(trajectorySize);

    for (int i = 1; i < trajectorySize - 1; ++i)
    {
        glm::vec3 p1 = m_trajectory[i - 1];
        glm::vec3 p2 = m_trajectory[i];
        glm::vec3 p3 = m_trajectory[i + 1];

        glm::vec3 a = p2 - p1;
        glm::vec3 b = p3 - p2;

        float product = a.x * b.y - a.y * b.x;

        m_isChangeVectorOrientation[i] = product > 0 ? true : false;
    }
}
