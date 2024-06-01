#version 430 core

uniform vec2 containerSize;
uniform vec3 color;
uniform vec3 outlineColor;

in vec2 TexCoords;

out vec4 FragColor;

uniform float borderWidth;

void main()
{
	vec2 borderWidthUV = vec2(borderWidth*2.0) / containerSize;

	
	vec2 distToCenter = abs(TexCoords - vec2(0.5)) * 2.0;
	vec2 summ = distToCenter + borderWidthUV;
	bool isBorder = summ.x >= 1.0 || summ.y >= 1.0;

	FragColor = vec4(isBorder ? outlineColor : color, 1.0);
}