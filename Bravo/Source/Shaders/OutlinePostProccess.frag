#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D OutlineColorMask;
uniform vec3 OutlineColor;

vec2 gridSampling[4] = vec2[]
(
    vec2(0, -2), vec2(0, 2), vec2(-2, 0), vec2(2, 0)
);
const int samples = 4;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(OutlineColorMask, 0));

    float BaseColor = texture(OutlineColorMask, TexCoords).r;
    for (int i = 0; i < samples; ++i)
    {
        vec2 sampleCoords = TexCoords + gridSampling[i] * texelSize;
        float OuterColor = texture(OutlineColorMask, sampleCoords).r;
        
        if ( OuterColor != BaseColor )
        {
            FragColor = vec4(OutlineColor, 1.0);
            return;
        }
    }
    discard;
}