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
    void renderTrajectory(const glm::mat4& viewMatrix);

private:
    void generateBuffers();

    ResourceManager* m_resourceManager;

    std::vector<glm::vec2> m_cut;
    std::vector<glm::vec3> m_trajectory;
    std::vector<float> m_cutParameters;

    GLuint m_vao;
    GLuint m_trajectoryBufferObject;

    std::shared_ptr<ShaderProgram> m_trajectoryShaderProgram;
};

#endif