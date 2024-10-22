#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertNormal;
out vec3 vertLightDir[15];
out vec3 vertHalfVector[15];

struct PointLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
};

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection_matrix;
	mat4 view_matrix;
};

layout (std140, binding = 1) uniform Lights
{
	vec4 globalAmbient;
	int lightsCount;
	PointLight light[15];
};

uniform mat4 model_matrix;

void main(void)
{
	mat4 normal_matrix = transpose(inverse(model_matrix));
	vertNormal = (normal_matrix * vec4(normal, 1.0)).xyz;

	vec3 mPos = (model_matrix * vec4(position, 1.0)).xyz;

	for (int i = 0; i < lightsCount; ++i)
	{
		vertLightDir[i] = light[i].position.xyz - mPos;
		vertHalfVector[i] = vertLightDir[i] - mPos;
	}

	gl_Position = projection_matrix * view_matrix * vec4(mPos, 1.0);
}