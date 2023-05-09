#version 330 core

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 aColor;

struct DirLight
{
	vec3 direction;
	vec3 lightColor;
	sampler2D shadowMap;
	mat4 lightSpaceMatrix;
};
struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 lightColor;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	vec3 lightColor;
	sampler2D shadowMap;
	mat4 lightSpaceMatrix;
};

uniform DirLight dirLight;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int pointLightsNum;

uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int spotLightsNum;

out VS_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;



void main()
{
	vs_out.FragPos = vec3(model * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	mat3 normalMatrix = mat3(model);
	vec3 T = normalize(vec3(normalMatrix * aTangent));
	vec3 N = normalize(vec3(normalMatrix * aNormal));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vs_out.TBN = mat3(T, B, N);


	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}