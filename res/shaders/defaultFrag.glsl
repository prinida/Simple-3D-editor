#version 460

out vec4 fragColor;

uniform vec3 color;

void main(void)
{
	fragColor = vec4(color, 1.0);
}