#ifndef SPHERE_H
#define SPHERE_H

#include <cmath>
#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
    Sphere();
    Sphere(int prec);
    
    int GetNumVertices();
    int GetNumIndices();
    std::vector<int>& GetIndices();
    std::vector<glm::vec3>& GetVertices();
    std::vector<glm::vec2>& GetTexCoords();
    std::vector<glm::vec3>& GetNormals();

private:
    void Init(int prec);
    float ToRadians(float degrees);

    const double PI = 3.1415926535897931;

    int numVertices;
    int numIndices;
    std::vector<int> indices;
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> texCoords;
    std::vector<glm::vec3> normals;
};

#endif