#version 460

layout(location = 0) in vec3 pos;

uniform mat4 projection_matrix;
uniform mat4 view_matrix;

void main(void)
{
	gl_Position = projection_matrix * view_matrix * vec4(pos, 1.0);
}

