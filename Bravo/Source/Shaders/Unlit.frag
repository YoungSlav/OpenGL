#version 430 core

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
    vec3 FragColor;
} fs_in;

out vec4 FragColor;

struct Material
{
    vec3 Color;
    sampler2D Texture;
    bool useTexture;
    bool useVertexColor;
};
uniform Material material;

void main()
{
    if ( material.useVertexColor )
    {
        FragColor = vec4(fs_in.FragColor.rgb, 1.0);
    }
    else if ( material.useTexture )
    {
        FragColor = vec4(texture(material.Texture, fs_in.TexCoords).rgb, 1.0);
    }
    else
    {
        FragColor = vec4(material.Color, 1.0);
    }
}