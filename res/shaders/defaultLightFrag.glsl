#version 460

in vec3 vertNormal;
in vec3 vertLightDir[15];
in vec3 vertHalfVector[15];

out vec4 fragColor;

struct PointLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec4 position;
};

struct NaturalMaterial
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

layout (std140, binding = 1) uniform Lights
{
	vec4 globalAmbient;
	int lightsCount;
	PointLight light[15];
};

uniform NaturalMaterial material;

void main(void)
{
	vec3 color = (globalAmbient * material.ambient).xyz;

    vec3 N = normalize(vertNormal);

    for (int i = 0; i < lightsCount; ++i)
	{
		vec3 L = normalize(vertLightDir[i]);
		vec3 H = normalize(vertHalfVector[i]);

		float cosTheta = dot(N,L);
    	float cosPhi = dot(H,N);

    	vec3 ambient = ((light[i].ambient * material.ambient).xyz);
    	vec3 diffuse = light[i].diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
    	vec3 specular = light[i].specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess * 3.0);

    	color += vec3(ambient + diffuse + specular);
	}
		
    fragColor = vec4(color, 1.0);
}