#version 460

layout(location = 0) in vec3 pos;

uniform mat4 view_matrix;

void main(void)
{
	gl_Position = view_matrix * vec4(pos, 1.0);
}

