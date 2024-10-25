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
    ReplicatedCutObject(std::string_view fullFilePath, ResourceManager* resourceManager, std::string_view material, std::string_view texture);
    ~ReplicatedCutObject();

    void setMaterial(std::string material);
    void setTexture(std::string texture);

    void prepareToRenderTrajectory();
    void renderTrajectory(const glm::vec3& color);

    void prepareToRenderTrajectoryCuts();
    void renderTrajectoryCuts(const glm::vec3& color, bool isFrameMode);

    void prepareToRenderReplicatedCut();
    void renderReplicatedCut(const glm::vec3& replicatedCutColor, const glm::vec3& normalsColor, bool isFrameMode, bool isLightEnabled, bool isNormalsMode, bool isSmoothNormalsMode);

private:
    void generateBuffers();
    void calcVectorsOrientationInTrajectory();

    void renderNormals(const glm::vec3& color, bool isSmoothMode);

    ResourceManager* m_resourceManager = nullptr;

    std::vector<glm::vec2> m_cut;
    std::vector<glm::vec2> m_originTranslatedCut;
    std::vector<glm::vec2> m_originTranslatedNormalizedCut;
    std::vector<glm::vec3> m_trajectory;
    std::vector<float> m_cutParameters;
    std::vector<glm::vec3> m_translatedCut;
    std::vector<glm::vec3> m_replicatedCut;
    std::vector<glm::vec3> m_replicatedCutNormals;
    std::vector<glm::vec3> m_replicatedCutSmoothedNormals;
    std::vector<glm::vec2> m_replicatedCutTextureCoords;

    std::vector<bool> m_isChangeVectorOrientation;

    GLuint m_vao{};
    GLuint m_trajectoryBufferObject{};
    GLuint m_trajectoryCutsBufferObject{};
    GLuint m_replicatedCutBufferObject{};
    GLuint m_replicatedCutNormalsBufferObject{};
    GLuint m_replicatedCutSmoothedNormalsBufferObject{};
    GLuint m_replicatedCutTextureBufferObject{};

    std::shared_ptr<ShaderProgram> m_defaultShaderProgram = nullptr;
    std::shared_ptr<ShaderProgram> m_defaultTextureShaderProgram = nullptr;
    std::shared_ptr<ShaderProgram> m_defaultLightShaderProgram = nullptr;
    std::shared_ptr<ShaderProgram> m_defaultNormalsShaderProgram = nullptr;

    bool m_isMaterialMode = true;
    std::shared_ptr<NaturalMaterial> m_material = nullptr;
    std::shared_ptr<Texture> m_texture = nullptr;
};

#endif