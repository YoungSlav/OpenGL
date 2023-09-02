#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 6) in mat4 instanceMatrix;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;

void main()
{
	vs_out.TexCoords = aTexCoords;
    gl_Position = projection * view * model * instanceMatrix * vec4(aPos, 1.0);
}