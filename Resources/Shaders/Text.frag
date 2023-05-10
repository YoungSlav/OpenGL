#version 330 core

in VS_OUT {
	vec4 FragPos;
	vec2 TexCoords;
} vs_in;
out vec4 color;

uniform sampler2D uTextAtlasTexture;
uniform vec4 uTextColor;

void main()
{
	vec4 sampled = vec4(1.0, 1.0, 1.0, texture(uTextAtlasTexture, vs_in.TexCoords).r);
    color = uTextColor * sampled;
}