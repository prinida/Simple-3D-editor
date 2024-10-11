#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertNormal;
out vec3 vertLightDir;
out vec3 vertHalfVector;

struct PointLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

uniform PointLight light;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;
uniform mat4 normal_matrix;

void main(void)
{
	vec3 viewPos = (view_matrix * vec4(position, 1.0)).xyz;
	vec3 lightPos = (view_matrix * vec4(light.position, 1.0)).xyz;

	vertLightDir = lightPos - viewPos;
	vertNormal = (normal_matrix * vec4(normal, 1.0)).xyz;
	vertHalfVector = vertLightDir - viewPos;

	gl_Position = projection_matrix * vec4(viewPos, 1.0);
}