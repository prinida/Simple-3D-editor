#version 460

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in vec3 vertNormal[];

layout (std140, binding = 0) uniform Matrices
{
	mat4 projection_matrix;
	mat4 view_matrix;
};

uniform mat4 model_matrix;

void main(void)
{
	mat4 mvp = projection_matrix * view_matrix * model_matrix;

	float normalLength = 0.03;

	for (int i = 0; i < 3; ++i)
	{
		vec4 p = gl_in[i].gl_Position;
		vec3 n = normalize(vertNormal[i]);

		gl_Position = mvp * p;
		EmitVertex();

		gl_Position = mvp * (p + vec4(normalLength * n, 0.0));
		EmitVertex();

		EndPrimitive();
	}
}