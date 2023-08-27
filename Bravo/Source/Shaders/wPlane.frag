#version 430 core

in VS_OUT {
	vec4 FragPos;
	vec2 TexCoords;
} vs_in;
out vec4 color;

uniform int uTextureBound = 0;
uniform sampler2D uTexture;
uniform vec4 uColor;

void main()
{
	if ( uTextureBound != 0 )
	{
		color = uColor * texture(uTexture, vs_in.TexCoords);
	}
	else
	{
		color = uColor;
	}
}