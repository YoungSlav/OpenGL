#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;
layout (location = 5) in vec4 aColor;

layout (location = 6) in mat4 instanceMatrix;


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
	vs_out.FragPos = vec3(model * instanceMatrix * vec4(aPos, 1.0));
	vs_out.TexCoords = aTexCoords;

	vec3 T = normalize(vec3(model * instanceMatrix * vec4(aTangent, 0.0)));
	vec3 N = normalize(vec3(model * instanceMatrix * vec4(aNormal, 0.0)));
	T = normalize(T - dot(T, N) * N);
	vec3 B = cross(N, T);
	vs_out.TBN = mat3(T, B, N);

	gl_Position = projection * view * vec4(vs_out.FragPos, 1.0);
}