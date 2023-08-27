#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

struct SpotLight
{
	mat4 lightSpaceMatrix;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;
  
	float constant;
	float linear;
	float quadratic;
	float farPlane;

};
layout(std430, binding = 1) buffer SpotLightBuffer
{
	SpotLight spotLights[];
};
uniform int spotLightCount = 0;

out vec4 FragPos; // FragPos from GS (output per emitvertex)

void main()
{
	for(int lightIndex = 0; lightIndex < spotLightCount; ++lightIndex)
	{
		gl_Layer = lightIndex; // built-in variable that specifies to which face we render.
		for(int i = 0; i < 3; ++i) // for each triangle's vertices
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = spotLights[lightIndex].lightSpaceMatrix * FragPos;
			EmitVertex();
		}    
		EndPrimitive();
	}
}