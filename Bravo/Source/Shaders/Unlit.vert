#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;
layout (location = 5) in vec4 aColor;

struct InstanceData
{
	mat4 transform;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	InstanceData instances[];
};


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
	vec3 FragColor;
} vs_out;

void main()
{
	vs_out.TexCoords = aTexCoords;
	vs_out.FragColor = aColor.rgb;
	
	gl_Position = projection * view * vec4(aPos, 1.0);
    gl_Position = projection * view * model * vec4(aPos, 1.0);
	gl_Position = projection * view * model * instances[gl_InstanceID].transform * vec4(aPos, 1.0);
}