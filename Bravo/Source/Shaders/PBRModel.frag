#version 430 core

struct InstanceData
{
	mat4 transform;
};

layout(std430, binding = 0) buffer InstanceBuffer
{
	InstanceData instances[];
};

struct Material
{
	sampler2D albedoTexture;
	bool useAlbedoTexture;
	vec3 albedoColor;
	
	sampler2D metallicTexture;
	bool useMetallicTexture;
	float metallicColor;

	sampler2D roughnessTexture;
	bool useRoughnessTexture;
	float roughnessColor;

	sampler2D aoTexture;
	bool useAoTexture;
	float aoColor;

	sampler2D normalTexture;
	bool useNormalTexture;

	sampler2D heightTexture;
	bool useHeightTexture;
	float heightScale;
};
uniform Material material;

struct DirectionalLight
{
	vec3 ambient;
	vec3 color;

	vec3 direction;

	int layerOffset;
	int cascadeCount;
	float cascadePlaneDistance;

	mat4 lightSpaceMatrix;
};
layout(std430, binding = 1) buffer DirectionalLightBuffer
{
	DirectionalLight dirLights[];
};
uniform int directionalLightCount = 0;
uniform sampler2DArray dirDepthMaps;

struct SpotLight
{
	vec3 color;

	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;

	float farPlane;

	mat4 lightSpaceMatrix;
};
layout(std430, binding = 2) buffer SpotLightBuffer
{
	SpotLight spotLights[];
};
uniform int spotLightCount = 0;
uniform sampler2DArray spotDepthMaps;

struct PointLight
{
	vec3 color;

	vec3 position;

	float farPlane;
};
layout(std430, binding = 3) buffer PointLightBuffer
{
	PointLight pointLights[];
};
uniform int pointLightCount;
uniform samplerCubeArray pointDepthMaps;

in VS_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoords;
} fs_in;
  
uniform mat4 model;
uniform mat4 view;
uniform vec3 viewPos;
uniform float drawDistance;

out vec4 FragColor;

const float PI = 3.14159265359;



// shadow function prototypes
float CalcDirLightShadow(int index, vec3 normal);
float CalcSpotLightShadow(int index, vec3 normal);
float CalcPointLightShadow(int index);


// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
	float a = roughness*roughness;
	float a2 = a*a;
	float NdotH = max(dot(N, H), 0.0);
	float NdotH2 = NdotH*NdotH;

	float nom   = a2;
	float denom = (NdotH2 * (a2 - 1.0) + 1.0);
	denom = PI * denom * denom;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
	float r = (roughness + 1.0);
	float k = (r*r) / 8.0;

	float nom   = NdotV;
	float denom = NdotV * (1.0 - k) + k;

	return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
	float NdotV = max(dot(N, V), 0.0);
	float NdotL = max(dot(N, L), 0.0);
	float ggx2 = GeometrySchlickGGX(NdotV, roughness);
	float ggx1 = GeometrySchlickGGX(NdotL, roughness);

	return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
	return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
vec3 calcLight(vec3 V, vec3 N, vec3 L, vec3 H, vec3 F0, vec3 albedo, float roughness, float metallic, vec3 radiance)
{
	// Cook-Torrance BRDF
	float NDF = DistributionGGX(N, H, roughness);   
	float G   = GeometrySmith(N, V, L, roughness);      
	vec3 F    = fresnelSchlick(clamp(dot(H, V), 0.0, 1.0), F0);
           
	vec3 numerator    = NDF * G * F; 
	float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001; // + 0.0001 to prevent divide by zero
	vec3 specular = numerator / denominator;
        
	// kS is equal to Fresnel
	vec3 kS = F;
	// for energy conservation, the diffuse and specular light can't
	// be above 1.0 (unless the surface emits light); to preserve this
	// relationship the diffuse component (kD) should equal 1.0 - kS.
	vec3 kD = vec3(1.0) - kS;
	// multiply kD by the inverse metalness such that only non-metals 
	// have diffuse lighting, or a linear blend if partly metal (pure metals
	// have no diffuse light).
	kD *= 1.0 - metallic;	  

	// scale light by NdotL
	float NdotL = max(dot(N, L), 0.0);        

	// add to outgoing radiance Lo
	return  (kD * albedo / PI + specular) * radiance * NdotL;  // note that we already multiplied the BRDF by the Fresnel (kS) so we won't multiply by kS again
}

vec2 parallaxMapping(vec2 texCoords, vec3 viewDir)
{
	float height = texture(material.heightTexture, texCoords).r;
	height = height * material.heightScale - material.heightScale * 0.5;
	vec2 pOffset = height * viewDir.xy / viewDir.z;
	return texCoords - pOffset;
}

void main()
{
	vec2 texCoords = fs_in.TexCoords;

	if ( material.useHeightTexture )
	{
		vec3 viewDirTangent = normalize(fs_in.TBN * (viewPos - fs_in.FragPos));
		texCoords = parallaxMapping(texCoords, viewDirTangent);
	}

	vec3 albedo = material.useAlbedoTexture ? texture(material.albedoTexture, texCoords).rgb : material.albedoColor;
	float metallic = material.useMetallicTexture ? texture(material.metallicTexture, texCoords).r : material.metallicColor;
	float roughness = material.useRoughnessTexture ? texture(material.roughnessTexture, texCoords).r : material.roughnessColor;
	float ao = material.useAoTexture ? texture(material.aoTexture, texCoords).r : material.aoColor;

	// properties
	vec3 N;
	
	if ( !material.useNormalTexture )
	{
		N = fs_in.TBN[2];
	}
	else
	{
		vec3 TexturenNorm = texture(material.normalTexture, texCoords).rgb;
		N = normalize(fs_in.TBN * normalize((TexturenNorm)*2.0 - 1.0));
	}
	vec3 V = normalize(viewPos - fs_in.FragPos);

	// calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
	// of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
	vec3 F0 = vec3(0.04); 
	F0 = mix(F0, albedo, metallic);

	// reflectance equation
	vec3 Lo = vec3(0.0);
	for ( int i = 0; i < directionalLightCount; i += dirLights[i].cascadeCount )
	{
		float shadow = CalcDirLightShadow(i, N);
		if ( shadow >= 1.0)
			continue;

		vec3 L = -dirLights[i].direction;
		vec3 H = normalize(V + L);

		float diff = max(dot(N, L), 0.0);

		vec3 radiance = dirLights[i].color * diff;
		Lo += (1.0 - shadow) * calcLight(V, N, L, H, F0, albedo, roughness, metallic, radiance);
	}


	for(int i = 0; i < spotLightCount; ++i) 
	{
		vec3 L = normalize(spotLights[i].position - fs_in.FragPos);
		float spotEffect = dot(L, -spotLights[i].direction);
		if (spotEffect <= spotLights[i].outerCutOff)
			continue;

		float shadow = CalcSpotLightShadow(i, N);
		if ( shadow >= 1.0)
			continue;

		vec3 H = normalize(V + L);
		float distance = length(spotLights[i].position - fs_in.FragPos);
		float epsilon = spotLights[i].cutOff - spotLights[i].outerCutOff;
		float coneAttenuation = clamp((spotEffect - spotLights[i].outerCutOff) / epsilon, 0.0, 1.0);
		
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = spotLights[i].color * attenuation * coneAttenuation;

		Lo += (1.0 - shadow) * calcLight(V, N, L, H, F0, albedo, roughness, metallic, radiance);
	}

	for(int i = 0; i < pointLightCount; ++i) 
	{
		float shadow = CalcPointLightShadow(i);
		if ( shadow >= 1.0)
			continue;
		// calculate per-light radiance
		vec3 L = normalize(pointLights[i].position - fs_in.FragPos);
		vec3 H = normalize(V + L);
		float distance = length(pointLights[i].position - fs_in.FragPos);
		float attenuation = 1.0 / (distance * distance);
		vec3 radiance = pointLights[i].color * attenuation;

		Lo += (1.0 - shadow) * calcLight(V, N, L, H, F0, albedo, roughness, metallic, radiance);
	}   
    
	// ambient lighting
	vec3 ambient = vec3(0.0);
	for ( int i = 0; i < directionalLightCount; i += dirLights[i].cascadeCount )
		ambient += dirLights[i].ambient * albedo * ao;

	vec3 color = ambient + Lo;

	// HDR tonemapping
	color = vec3(1.0) - exp(-color * 2.2);
	// gamma correct
	color = pow(color, vec3(1.0/2.2)); 

	FragColor = vec4(color, 1.0);
}

float CalcDirLightShadow(int index, vec3 normal)
{
	// select cascade layer
	vec4 fragPosViewSpace = view * vec4(fs_in.FragPos, 1.0);
	float depthValue = abs(fragPosViewSpace.z);

	int layer = -1;
	for (int i = index; i < index + dirLights[index].cascadeCount; ++i)
	{
		if (depthValue < dirLights[i].cascadePlaneDistance)
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = index + dirLights[index].cascadeCount-1;
	}

	vec4 fragPosLightSpace = dirLights[layer].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
		return 0.0;
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(normal, -dirLights[layer].direction)), 0.05);
	const float biasModifier = 0.5f;
	bias *= 1 / (dirLights[layer].cascadePlaneDistance * biasModifier);
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(dirDepthMaps, 0));
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(dirDepthMaps, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
		}    
	}
	shadow /= 9.0;
	return shadow;
}

float CalcSpotLightShadow(int index, vec3 normal)
{
	vec4 fragPosLightSpace = spotLights[index].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	vec3 lightDir = normalize(spotLights[index].position - fs_in.FragPos);
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
		return 0.0;
	
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;      
	
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(normal, -lightDir)), 0.005);
	const float biasModifier = 1.0f;
	bias *= 1 / (spotLights[index].farPlane * biasModifier);
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(spotDepthMaps, 0));
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(spotDepthMaps, vec3(projCoords.xy + vec2(x, y) * texelSize, index)).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}

float CalcPointLightShadow(int index)
{
	vec3 gridSamplingDisk[20] = vec3[]
	(
		vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
		vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
		vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
		vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
		vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
	);
	int samples = 20;


	// get vector between fragment position and light position
	vec3 fragToLight = fs_in.FragPos - pointLights[index].position;
	// now get current linear depth as the length between the fragment and light position
	float currentDepth = length(fragToLight);
	// now test for shadows
	const float bias = 0.075;
	
	float shadow = 0.0;
	float viewDistance = length(viewPos - fs_in.FragPos);
	float diskRadius = (1.0 + (viewDistance / drawDistance)) / pointLights[index].farPlane;
	for(int i = 0; i < samples; ++i)
	{
		vec3 sampleCoord = fragToLight + gridSamplingDisk[i] * diskRadius;
		float pcfDepth = texture(pointDepthMaps, vec4(sampleCoord, index)).r;
		pcfDepth *= pointLights[index].farPlane;   // undo mapping [0;1]
		shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
	}
	shadow /= float(samples);

    return shadow;
}