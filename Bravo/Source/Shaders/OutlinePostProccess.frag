#version 430 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D OutlineColorMask;
uniform sampler2D OutlineDepthMask;

uniform float nearPlane;
uniform float farPlane;


vec2 gridSampling[4] = vec2[]
	(
		vec2(0, -1), vec2( 0, 1), vec2(-1, 0), vec2(1, 0)
	);
const int samples = 4;

float LinearizeDepth(float Depth)
{
    const float near = nearPlane;
    const float far = farPlane;
    const float linearDepth = (2.0 * near) / (far + near - Depth * (far - near));
    return linearDepth;
}

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(OutlineColorMask, 0));

    vec3 BaseColor = texture(OutlineColorMask, TexCoords).rgb;
    vec3 OuterColors[samples];
    bool bShouldDiscard = true;
    for ( int i = 0; i < samples; ++i )
    {
        OuterColors[i] = texture(OutlineColorMask, TexCoords + gridSampling[i]*texelSize).rgb;
        if ( OuterColors[i] != BaseColor )
            bShouldDiscard = false;
    }
    if ( bShouldDiscard )
        discard;

    FragColor = vec4(BaseColor, 1.0);
    return;
    if ( BaseColor != vec3(0.0) )
    {
        FragColor = vec4(BaseColor, 1.0);
    }

    const float DepthThreashold = 0.01;
    
    float BaseDepth = LinearizeDepth(texture(OutlineDepthMask, TexCoords).r);
    for ( int i = 0; i < samples; ++i )
    {
        if ( OuterColors[i] == vec3(0.0) )
            continue;
        float OffsetDepth = LinearizeDepth(texture(OutlineDepthMask, TexCoords + gridSampling[i]*texelSize).r);
        float DepthDiff = abs(OffsetDepth - BaseDepth);
        if ( DepthDiff >= DepthThreashold )
        {
            FragColor = vec4(BaseColor, 1.0);
            break;
        }
    }
} 