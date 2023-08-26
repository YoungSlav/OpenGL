#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform int depthMapLayer;

void main()
{
	for (int i = 0; i < 3; ++i)
	{
		gl_Layer = depthMapLayer;//gl_InvocationID;
		gl_Position = gl_in[i].gl_Position;
		EmitVertex();
	}
	EndPrimitive();
}