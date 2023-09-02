#version 430 core

in VS_OUT {
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

out vec4 FragColor;

struct Material
{
    vec3 Color;
    sampler2D Texture;
    bool useTexture;
};
uniform Material material;

void main()
{             
    FragColor = vec4(material.useTexture ? texture(material.Texture, fs_in.TexCoords).rgb : material.Color, 1.0);
}