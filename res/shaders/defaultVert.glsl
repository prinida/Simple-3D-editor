#version 460

layout(location = 0) in vec3 pos;

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection_matrix;
	mat4 view_matrix;
};

void main(void)
{
	gl_Position = projection_matrix * view_matrix * vec4(pos, 1.0);
}

