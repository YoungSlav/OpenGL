#version 430 core

uniform vec2 WorldSize;
uniform mat4 ModelInverse;

uniform float TargetDensity;
uniform float DensityOffset;

uniform float LightMarchingStep;

uniform vec2 DirToLight;
uniform vec3 ScatteringCoefficients;

uniform sampler2D DensitiesMap;

in VS_OUT {
	vec2 TexCoords;
	vec2 WorldPos;
} vs_out;


out vec4 FragColor;

float SampleDensity(vec2 myPosition);

void main()
{
    float Density = SampleDensity(vs_out.WorldPos);
    FragColor = vec4(Density, 0.0f, 0.0f, 0.5f);
}

float SampleDensity(vec2 position)
{
    vec2 localPos = (ModelInverse * vec4(position, 0.0f, 1.0f)).xy;
    vec2 samplePos = localPos + vec2(0.5);
    samplePos.x = 1.0 - samplePos.x;
    return texture(DensitiesMap, samplePos).r;
}
