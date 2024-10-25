#version 460

in vec2 vertTex;
out vec4 fragColor;

layout (binding = 2) uniform sampler2D texSamp;

void main(void)
{
	fragColor = texture(texSamp, vertTex);
}