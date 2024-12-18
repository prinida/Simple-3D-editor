#version 460

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

out vec3 vertNormal;

void main(void)
{
	vertNormal = normal;
	gl_Position = vec4(position, 1.0);
}