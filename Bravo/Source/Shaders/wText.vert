#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aTexCoords;

out VS_OUT {
	vec4 FragPos;
	vec2 TexCoords;
} vs_out;


uniform mat4 model;

void main()
{
	vs_out.FragPos = model * vec4(aPos, 1.0, 1.0);
	vs_out.TexCoords = aTexCoords;

	gl_Position = vs_out.FragPos;
}