#ifndef REPLICATED_CUT_OBJECT_H
#define REPLICATED_CUT_OBJECT_H

#include "ResourcesManager.h"
#include "ShaderProgram.h"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <memory>
#include <string_view>
#include <vector>

class ReplicatedCutObject
{
public:
    ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager);
    ~ReplicatedCutObject();

    void prepareToRenderTrajectory();
    void renderTrajectory(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& color);

    void prepareToRenderTrajectoryCuts();
    void renderTrajectoryCuts(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& color);

    void prepareToRenderReplicatedCut();
    void renderReplicatedCut(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix, const glm::vec3& color);

private:
    void generateBuffers();
    void calcVectorsOrientationInTrajectory();

    ResourceManager* m_resourceManager = nullptr;

    std::vector<glm::vec2> m_cut;
    std::vector<glm::vec3> m_trajectory;
    std::vector<float> m_cutParameters;
    std::vector<glm::vec3> m_translatedCut;
    std::vector<glm::vec3> m_replicatedCut;

    std::vector<bool> m_isChangeVectorOrientation;

    GLuint m_vao{};
    GLuint m_trajectoryBufferObject{};
    GLuint m_trajectoryCutsBufferObject{};
    GLuint m_replicatedCutBufferObject{};

    std::shared_ptr<ShaderProgram> m_defaultShaderProgram = nullptr;
};

#endif