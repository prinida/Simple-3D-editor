#version 460

in vec3 vertNormal;
in vec3 vertLightDir;
in vec3 vertHalfVector;

out vec4 fragColor;

struct PointLight
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	vec3 position;
};

struct NaturalMaterial
{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
};

uniform PointLight light;
uniform NaturalMaterial material;
uniform vec4 globalAmbient;

void main(void)
{
	vec3 L = normalize(vertLightDir);
    vec3 N = normalize(vertNormal);
    vec3 H = normalize(vertHalfVector);

    float cosTheta = dot(N,L);
    float cosPhi = dot(H,N);

    vec3 ambient = (((globalAmbient + light.ambient) * material.ambient).xyz);
    vec3 diffuse = light.diffuse.xyz * material.diffuse.xyz * max(cosTheta, 0.0);
    vec3 specular = light.specular.xyz * material.specular.xyz * pow(max(cosPhi, 0.0), material.shininess * 3.0);

    fragColor = vec4(ambient + diffuse + specular, 1.0);
}