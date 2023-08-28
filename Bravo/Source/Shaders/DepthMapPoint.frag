#version 430 core
in vec4 FragPos;

uniform vec3 position;
uniform float farPlane;

void main()
{
	// get distance between fragment and light source
	float lightDistance = length(FragPos.xyz - position);
    
	// map to [0;1] range by dividing by far_plane
	lightDistance = lightDistance / farPlane;
    
	// write this as modified depth
	gl_FragDepth = lightDistance;
}  