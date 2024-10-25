#version 460

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 tex;

out vec2 vertTex;

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection_matrix;
	mat4 view_matrix;
};

uniform mat4 model_matrix;

void main(void)
{
	vertTex = tex;
	gl_Position = projection_matrix * view_matrix * model_matrix * vec4(pos, 1.0);
}

