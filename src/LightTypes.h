#ifndef LIGHT_TYPES_H
#define LIGHT_TYPES_H

#include <glm/glm.hpp>

struct GlobalAmbientLight
{
    glm::vec4 ambient{};
};

struct PointLight
{
    glm::vec4 ambient{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
    glm::vec4 position{};
};

#endif