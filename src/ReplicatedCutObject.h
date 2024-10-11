#ifndef REPLICATED_CUT_OBJECT_H
#define REPLICATED_CUT_OBJECT_H

#include "LightManager.h"
#include "MaterialTypes.h"
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
    ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager, LightManager* lightManager, std::string_view material);
    ~ReplicatedCutObject();

    void prepareToRenderTrajectory();
    void renderTrajectory(const glm::vec3& color);

    void prepareToRenderTrajectoryCuts();
    void renderTrajectoryCuts(const glm::vec3& color, bool isFrameMode);

    void prepareToRenderReplicatedCut();
    void renderReplicatedCut(const glm::vec3& color, bool isFrameMode, bool isLightEnabled);

    void renderNormals(const glm::vec3& color, bool isSmoothMode);

private:
    void generateBuffers();
    void calcVectorsOrientationInTrajectory();

    ResourceManager* m_resourceManager = nullptr;
    LightManager* m_lightManager = nullptr;

    std::vector<glm::vec2> m_cut;
    std::vector<glm::vec3> m_trajectory;
    std::vector<float> m_cutParameters;
    std::vector<glm::vec3> m_translatedCut;
    std::vector<glm::vec3> m_replicatedCut;
    std::vector<glm::vec3> m_replicatedCutNormals;
    std::vector<glm::vec3> m_normals;

    std::vector<bool> m_isChangeVectorOrientation;

    GLuint m_vao{};
    GLuint m_trajectoryBufferObject{};
    GLuint m_trajectoryCutsBufferObject{};
    GLuint m_replicatedCutBufferObject{};
    GLuint m_replicatedCutNormalsBufferObject{};
    GLuint m_normalsBufferObject{};

    std::shared_ptr<ShaderProgram> m_defaultShaderProgram = nullptr;
    std::shared_ptr<ShaderProgram> m_defaultLightShaderProgram = nullptr;

    std::shared_ptr<NaturalMaterial> m_material = nullptr;
};

#endif