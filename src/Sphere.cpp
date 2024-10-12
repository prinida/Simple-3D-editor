#include "Sphere.h"

Sphere::Sphere()
{
    Init(48);
}

Sphere::Sphere(int prec)
{
    Init(prec);
}

int Sphere::GetNumVertices()
{
    return numVertices;
}

int Sphere::GetNumIndices()
{
    return numIndices;
}

std::vector<int>& Sphere::GetIndices()
{
    return indices;
}

std::vector<glm::vec3>& Sphere::GetVertices()
{
    return vertices;
}

std::vector<glm::vec2>& Sphere::GetTexCoords()
{
    return texCoords;
}

std::vector<glm::vec3>& Sphere::GetNormals()
{
    return normals;
}

void Sphere::Init(int prec)
{
    numVertices = (prec + 1) * (prec + 1);
    numIndices = prec * prec * 6;

    indices.resize(numIndices, 0);
    vertices.resize(numVertices, glm::vec3());
    texCoords.resize(numVertices, glm::vec2());
    normals.resize(numVertices, glm::vec3());

    for (int i = 0; i <= prec; i++)
    {
        float y = cos(ToRadians(180.0f - i * 180.0f / prec));

        for (int j = 0; j <= prec; j++)
        {
            float x = -cos(ToRadians(j * 360.0f / prec)) * abs(cos(asin(y)));
            float z = sin(ToRadians(j * 360.0f / prec)) * abs(cos(asin(y)));

            int index = i * (prec + 1) + j;

            vertices[index] = glm::vec3(x, y, z);
            normals[index] = glm::vec3(x, y, z);
            texCoords[index] = glm::vec2((float)j / prec, (float)i / prec);
        }
    }

    for (int i = 0; i < prec; i++)
    {
        for (int j = 0; j < prec; j++)
        {
            int index = 6 * (i * prec + j);

            int firstPoint = i * (prec + 1) + j;
            int secondPoint = i * (prec + 1) + j + 1;
            int thirdPoint = (i + 1) * (prec + 1) + j + 1;
            int fourthPoint = (i + 1) * (prec + 1) + j;

            indices[index] = firstPoint;
            indices[index + 1] = secondPoint;
            indices[index + 2] = fourthPoint;

            indices[index + 3] = secondPoint;
            indices[index + 4] = thirdPoint;
            indices[index + 5] = fourthPoint;
        }
    }
}

float Sphere::ToRadians(float degrees)
{
    return degrees * PI / 180.0f;
}
