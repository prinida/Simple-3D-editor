#include "ReplicatedCutObject.h"

#include "ResourcesManager.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

#include <fstream>
#include <iostream>
#include <string_view>

ReplicatedCutObject::ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager, std::string_view material)
{
    m_resourceManager = resourceManager;

    m_defaultShaderProgram = m_resourceManager->getShaderProgram("defaultSP");
    m_defaultLightShaderProgram = m_resourceManager->getShaderProgram("defaultLightSP");
    m_defaultNormalsShaderProgram = m_resourceManager->getShaderProgram("defaultNormalsSP");

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
    glDeleteBuffers(1, &m_replicatedCutSmoothedNormalsBufferObject);
}

void ReplicatedCutObject::setMaterial(std::string material)
{
    m_material = m_resourceManager->getNaturalMaterial(material);
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
    m_defaultShaderProgram->setMat4("model_matrix", glm::mat4(1.0f));

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

    m_originTranslatedCut.resize(cutSize);

    glm::vec3 cutCenter{};
    for (int i = 0; i < cutSize; ++i)
        cutCenter += glm::vec3(m_cut[i], 0.0f);
    cutCenter /= cutSize;

    glm::mat4 tran = glm::translate(glm::mat4(1.0f), -cutCenter);
    for (int i = 0; i < cutSize; ++i)
    {
        m_originTranslatedCut[i] = glm::vec2(tran * glm::vec4(m_cut[i], 0.0f, 1.0f));
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

        float scaleParam = m_cutParameters[i];
        glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(scaleParam));

        for (int j = 0; j < cutSize; ++j)
        {
            glm::vec2 scaledVertex = glm::vec2(scale * glm::vec4(m_originTranslatedCut[j], 0.0f, 1.0f));
            m_translatedCut[j + shift] = rotate * glm::vec3(scaledVertex, 0.0f) + translate;
        }

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
    m_defaultShaderProgram->setMat4("model_matrix", glm::mat4(1.0f));

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

    m_replicatedCut.resize(replicatedCutSize);
    m_replicatedCutNormals.resize(replicatedCutSize);
    m_replicatedCutSmoothedNormals.resize(replicatedCutSize);

    int repCutIndex = 0;

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

            glm::vec3 normal2first = glm::cross(point1 - point2, point3 - point2);
            glm::vec3 normal2second = glm::cross(point3 - point2, point4 - point2);
            glm::vec3 normal3first = glm::cross(point1 - point3, point2 - point3);
            glm::vec3 normal3second = glm::cross(point2 - point3, point4 - point3);

            glm::vec3 normal1 = glm::cross(point2 - point1, point3 - point1);
            glm::vec3 normal2 = (normal2first + normal2second) / 2.0f;
            glm::vec3 normal3 = (normal3first + normal3second) / 2.0f;
            glm::vec3 normal4 = glm::cross(point2 - point4, point3 - point4);

            m_replicatedCutNormals[repCutIndex] = -normal1;
            m_replicatedCutNormals[repCutIndex + 1] = normal2;
            m_replicatedCutNormals[repCutIndex + 2] = -normal3;
            m_replicatedCutNormals[repCutIndex + 3] = normal2;
            m_replicatedCutNormals[repCutIndex + 4] = -normal3;
            m_replicatedCutNormals[repCutIndex + 5] = normal4;

            repCutIndex += 6;
        }
    }

    glm::vec3 center0 = m_translatedCut[0];
    glm::vec3 normal = m_trajectory[0] - m_trajectory[1];

    for (int i = 1; i < cutSize + 1; ++i)
    {
        m_replicatedCut[repCutIndex] = center0;
        m_replicatedCut[repCutIndex + 1] = m_translatedCut[i];
        m_replicatedCut[repCutIndex + 2] = m_translatedCut[i + 1];

        m_replicatedCutNormals[repCutIndex] = normal;
        m_replicatedCutNormals[repCutIndex + 1] = normal;
        m_replicatedCutNormals[repCutIndex + 2] = normal;

        repCutIndex += 3;
    }

    glm::vec3 center1 = m_translatedCut[m_translatedCut.size() - cutSize - 2];
    normal = m_trajectory[m_trajectory.size() - 1] - m_trajectory[m_trajectory.size() - 2];

    int endCutIndex = m_translatedCut.size() - cutSize - 1;

    for (int i = 0; i < cutSize; ++i)
    {
        m_replicatedCut[repCutIndex] = center1;
        m_replicatedCut[repCutIndex + 1] = m_translatedCut[endCutIndex + i];
        m_replicatedCut[repCutIndex + 2] = m_translatedCut[endCutIndex + i + 1];

        m_replicatedCutNormals[repCutIndex] = normal;
        m_replicatedCutNormals[repCutIndex + 1] = normal;
        m_replicatedCutNormals[repCutIndex + 2] = normal;

        repCutIndex += 3;
    }

    for (int i = 0; i < cutNum - 1; ++i)
    {
        int cutIndex = i * cutSize * 6;
        int prevCutIndex = (i - 1) * cutSize * 6;
        int nextCutIndex = (i + 1) * cutSize * 6;

        if (i == 0)
            prevCutIndex = 0;

        for (int j = 0; j < cutSize; ++j)
        {
            int nextj = j + 1;
            if (nextj == cutSize)
                nextj = 0;

            int prevj = j - 1;
            if (prevj == -1)
                prevj = cutSize - 1;

            int rectIndex = cutIndex + j * 6;
            int nextjRectIndex = cutIndex + nextj * 6;
            int prevjRectIndex = cutIndex + prevj * 6;

            int previrectIndex = prevCutIndex + j * 6;
            int previnextjRectIndex = prevCutIndex + nextj * 6;
            int previprevjRectIndex = prevCutIndex + prevj * 6;

            int nextirectIndex = nextCutIndex + j * 6;
            int nextinextjRectIndex = nextCutIndex + nextj * 6;
            int nextiprevjRectIndex = nextCutIndex + prevj * 6;

            glm::vec3 normal1_1 = glm::normalize(m_replicatedCutNormals[rectIndex]);
            glm::vec3 normal1_2 = glm::normalize(m_replicatedCutNormals[prevjRectIndex + 1]);
            glm::vec3 normal1_3 = glm::normalize(m_replicatedCutNormals[previrectIndex + 2]);
            glm::vec3 normal1_4 = glm::normalize(m_replicatedCutNormals[previprevjRectIndex + 5]);

            glm::vec3 normal2_1 = glm::normalize(m_replicatedCutNormals[nextjRectIndex]);
            glm::vec3 normal2_2 = glm::normalize(m_replicatedCutNormals[rectIndex + 1]);
            glm::vec3 normal2_3 = glm::normalize(m_replicatedCutNormals[previnextjRectIndex + 2]);
            glm::vec3 normal2_4 = glm::normalize(m_replicatedCutNormals[previrectIndex + 5]);

            glm::vec3 normal3_1 = glm::normalize(m_replicatedCutNormals[nextirectIndex]);
            glm::vec3 normal3_2 = glm::normalize(m_replicatedCutNormals[nextiprevjRectIndex + 1]);
            glm::vec3 normal3_3 = glm::normalize(m_replicatedCutNormals[rectIndex + 2]);
            glm::vec3 normal3_4 = glm::normalize(m_replicatedCutNormals[prevjRectIndex + 5]);

            glm::vec3 normal4_1 = glm::normalize(m_replicatedCutNormals[nextinextjRectIndex]);
            glm::vec3 normal4_2 = glm::normalize(m_replicatedCutNormals[nextirectIndex + 1]);
            glm::vec3 normal4_3 = glm::normalize(m_replicatedCutNormals[nextjRectIndex + 2]);
            glm::vec3 normal4_4 = glm::normalize(m_replicatedCutNormals[rectIndex + 5]);

            glm::vec3 smoothedNormal1 = (normal1_1 + normal1_2 + normal1_3 + normal1_4) / 4.0f;
            glm::vec3 smoothedNormal2 = (normal2_1 + normal2_2 + normal2_3 + normal2_4) / 4.0f;
            glm::vec3 smoothedNormal3 = (normal3_1 + normal3_2 + normal3_3 + normal3_4) / 4.0f;
            glm::vec3 smoothedNormal4 = (normal4_1 + normal4_2 + normal4_3 + normal4_4) / 4.0f;

            if (i == 0)
            {
                int startCutIndex = (cutNum - 1) * cutSize * 6;
                int startTriangleIndex = startCutIndex + 3 * j;

                glm::vec3 normal = glm::normalize(m_replicatedCutNormals[startCutIndex]);

                smoothedNormal1 = (normal1_1 + normal1_2 + normal + normal) / 4.0f;
                smoothedNormal2 = (normal2_1 + normal2_2 + normal + normal) / 4.0f;

                m_replicatedCutSmoothedNormals[startTriangleIndex] = normal;
                m_replicatedCutSmoothedNormals[startTriangleIndex + 1] = smoothedNormal1;
                m_replicatedCutSmoothedNormals[startTriangleIndex + 2] = smoothedNormal2;
            }
            else if (i == cutNum - 2)
            {
                int endCutIndex = ((cutNum - 1) + 1.0 / 2.0) * cutSize * 6;
                int startTriangleIndex = endCutIndex + 3 * j;

                glm::vec3 normal = glm::normalize(m_replicatedCutNormals[endCutIndex]);

                smoothedNormal3 = (normal + normal + normal3_3 + normal3_4) / 4.0f;
                smoothedNormal4 = (normal + normal + normal4_3 + normal4_4) / 4.0f;

                m_replicatedCutSmoothedNormals[startTriangleIndex] = normal;
                m_replicatedCutSmoothedNormals[startTriangleIndex + 1] = smoothedNormal3;
                m_replicatedCutSmoothedNormals[startTriangleIndex + 2] = smoothedNormal4;
            }

            m_replicatedCutSmoothedNormals[rectIndex] = smoothedNormal1;
            m_replicatedCutSmoothedNormals[rectIndex + 1] = smoothedNormal2;
            m_replicatedCutSmoothedNormals[rectIndex + 2] = smoothedNormal3;
            m_replicatedCutSmoothedNormals[rectIndex + 3] = smoothedNormal2;
            m_replicatedCutSmoothedNormals[rectIndex + 4] = smoothedNormal3;
            m_replicatedCutSmoothedNormals[rectIndex + 5] = smoothedNormal4;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_replicatedCut.size() * sizeof(float) * 3, m_replicatedCut.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutNormalsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_replicatedCutNormals.size() * sizeof(float) * 3, m_replicatedCutNormals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutSmoothedNormalsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, m_replicatedCutSmoothedNormals.size() * sizeof(float) * 3, m_replicatedCutSmoothedNormals.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ReplicatedCutObject::renderReplicatedCut(const glm::vec3& replicatedCutColor, const glm::vec3& normalsColor, bool isFrameMode, bool isLightEnabled, bool isNormalsMode, bool isSmoothNormalsMode)
{
    if (isLightEnabled)
    {
        if (isNormalsMode && isSmoothNormalsMode)
            renderNormals(normalsColor, true);
        else if(isNormalsMode && !isSmoothNormalsMode)
            renderNormals(normalsColor, false);

        m_defaultLightShaderProgram->use();

        m_defaultLightShaderProgram->setMat4("model_matrix", glm::mat4(1.0f));

        // это должно быть где-то снаружи!
        m_defaultLightShaderProgram->setVec4("material.ambient", m_material->ambient);
        m_defaultLightShaderProgram->setVec4("material.diffuse", m_material->diffuse);
        m_defaultLightShaderProgram->setVec4("material.specular", m_material->specular);
        m_defaultLightShaderProgram->setFloat("material.shininess", m_material->shininess);

        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        if (isSmoothNormalsMode)
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutSmoothedNormalsBufferObject);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutNormalsBufferObject);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
            glEnableVertexAttribArray(1);
        }
    }
    else
    {
        glBindVertexArray(m_vao);

        glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        m_defaultShaderProgram->use();
        m_defaultShaderProgram->setVec3("color", replicatedCutColor);
        m_defaultShaderProgram->setMat4("model_matrix", glm::mat4(1.0f));
    }

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
    glEnable(GL_LINE_SMOOTH);

    m_defaultNormalsShaderProgram->use();

    m_defaultNormalsShaderProgram->setVec3("color", color);
    m_defaultNormalsShaderProgram->setMat4("model_matrix", glm::mat4(1.0f));

    glBindVertexArray(m_vao);

    glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutBufferObject);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (isSmoothMode)
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutSmoothedNormalsBufferObject);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }
    else
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_replicatedCutNormalsBufferObject);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(1);
    }

    glDrawArrays(GL_TRIANGLES, 0, m_replicatedCut.size());

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
    glCreateBuffers(1, &m_replicatedCutSmoothedNormalsBufferObject);
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
