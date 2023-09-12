#version 430 core

uniform int ObjectHandle;
flat in float InstanceID;

out vec4 FragColor;


void main()
{             
    FragColor = vec4(vec3(ObjectHandle, InstanceID, 0.0), 1.0);
}