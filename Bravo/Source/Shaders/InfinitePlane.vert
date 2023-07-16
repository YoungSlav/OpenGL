#version 410 core
layout (location = 0) in vec3 aPos;

out vec3 nearPoint;
out vec3 farPoint;


uniform mat4 projection;
uniform mat4 view;


vec3 UnprojectPoint(float x, float y, float z)
{
    mat4 viewInv = inverse(view);
    mat4 projInv = inverse(projection);
    vec4 unprojectedPoint =  viewInv * projInv * vec4(x, y, z, 1.0);
    return unprojectedPoint.xyz / unprojectedPoint.w;
}

void main()
{
    nearPoint = UnprojectPoint(aPos.x, aPos.y, 0.0).xyz;
    farPoint = UnprojectPoint(aPos.x, aPos.y, 1.0).xyz;

	gl_Position = vec4(aPos, 1.0f);
}