#version 430 core



in VS_OUT {
	vec2 TexCoords;
	vec2 WorldPos;
} vs_out;


out vec4 FragColor;

uniform vec2 containerSize;
uniform vec3 outlineColor;

uniform float interactionRadius;
uniform float interactionForce;
uniform float gravityForce;
uniform float borderWidth;
uniform vec2 mousePos;

void main()
{
	// border
	vec2 borderWidthUV = vec2(borderWidth) / containerSize;
	vec2 distToCenter = abs(vs_out.TexCoords - vec2(0.5)) * 2.0f;
	vec2 summ = distToCenter + borderWidthUV;
	bool isBorder = summ.x >= 1.0 || summ.y >= 1.0;
	if ( isBorder )
	{
		FragColor = vec4(outlineColor, 1.0);
		return;
	}

	// interaction
	vec2 mouseOffset = mousePos - vs_out.WorldPos;
	float mouseDst2 = dot(mouseOffset, mouseOffset);
	if ( mouseDst2 <= interactionRadius*interactionRadius )
	{
		if ( abs(mouseDst2 - interactionRadius*interactionRadius) <= 0.01f )
		{
			FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0);
			return;
		}
		else
		{

			// TODO: display with accounting to interactionForce and gravityForce
			float dst = sqrt(mouseDst2);
			float normilizedInteractionForce =interactionForce / (gravityForce*10.0f);
			float normilizeDist = (1.0f - (dst / interactionRadius));

			float totalMix = normilizeDist * normilizedInteractionForce;


			vec3 color = mix(vec3(0.0), vec3(1.0, 0.0, 0.0), totalMix);
			FragColor = vec4(color, 1.0);
			return;
		}
	}

	FragColor = vec4(vec3(0.0f), 1.0);
}