#version 430 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform int layerOffset;
uniform mat4 lightSpaceMatricies[6];

out vec4 FragPos;

void main()
{
	for(int face = 0; face < 6; ++face)
	{
		gl_Layer = face + layerOffset*6;
		for(int i = 0; i < 3; ++i) // for each triangle's vertices
		{
			FragPos = gl_in[i].gl_Position;
			gl_Position = lightSpaceMatricies[face] * FragPos;
			EmitVertex();
		}    
		EndPrimitive();
	}
}