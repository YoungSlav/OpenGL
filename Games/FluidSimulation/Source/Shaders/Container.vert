#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;


uniform mat4 camera;
uniform mat4 model;

out VS_OUT {
	vec2 TexCoords;
	vec2 WorldPos;
} vs_out;


void main()
{
	vec4 worldPos = model * vec4(aPos, 0.0, 1.0);
	vs_out.TexCoords = aTexCoords;
	vs_out.WorldPos = worldPos.xy;

	gl_Position = camera * worldPos;
}