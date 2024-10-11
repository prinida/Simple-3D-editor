#ifndef MATERIAL_TYPES_H
#define MATERIAL_TYPES_H

#include <glm/glm.hpp>

struct NaturalMaterial
{
    glm::vec4 ambient{};
    glm::vec4 diffuse{};
    glm::vec4 specular{};
    float shininess = 0;
};

#endif