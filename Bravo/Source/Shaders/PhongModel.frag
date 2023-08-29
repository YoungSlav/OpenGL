#version 430 core

struct Material
{
	int diffuseTexture;
	vec3 diffuseColor;
	
	int specularTexture;
	vec3 specularColor;
	
	int normalTexture;

	float shininess;

	sampler2DArray textureArray;
};
uniform Material material;

struct DirectionalLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;

	int layerOffset;
	int cascadeCount;
	float cascadePlaneDistance;

	mat4 lightSpaceMatrix;
};
layout(std430, binding = 0) buffer DirectionalLightBuffer
{
	DirectionalLight dirLights[];
};
uniform int directionalLightCount = 0;
uniform sampler2DArray dirDepthMaps;



struct SpotLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;

	float cutOff;
	float outerCutOff;
  
	float constant;
	float linear;
	float quadratic;
	float farPlane;

	mat4 lightSpaceMatrix;
};
layout(std430, binding = 1) buffer SpotLightBuffer
{
	SpotLight spotLights[];
};
uniform int spotLightCount = 0;
uniform sampler2DArray spotDepthMaps;




struct PointLight
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;

	float constant;
	float linear;
	float quadratic;

	float farPlane;
};
layout(std430, binding = 2) buffer PointLightBuffer
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
  
uniform mat4 view;
uniform vec3 viewPos;
uniform float drawDistance;




out vec4 FragColor;


// function prototypes
vec3 CalcDirLight(int index, vec3 normal, vec3 viewDir);
float CalcDirLightShadow(int index, vec3 norm);

vec3 CalcSpotLight(int index, vec3 normal, vec3 viewDir);
float CalcSpotLightShadow(vec4 fragPosLightSpace, int depthMapLayer, vec3 norm, vec3 lightDir, float FarPlane);

vec3 CalcPointLight(int index, vec3 normal, vec3 viewDir);
float CalcPointLightShadow(int index);


void main()
{
	// properties
	vec3 norm;
	
	if ( material.normalTexture == -1 )
	{
		norm = fs_in.TBN[2];
	}
	else
	{
		vec3 TexturenNorm = texture(material.textureArray, vec3(fs_in.TexCoords,material.normalTexture) ).rgb;
		norm = normalize(fs_in.TBN * normalize((TexturenNorm)*2.0 - 1.0));
	}

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
	vec3 outColor = vec3(0);

	for ( int i = 0; i < directionalLightCount; i += dirLights[i].cascadeCount )
	{
		outColor += CalcDirLight(i, norm, viewDir);
	}

	for(int i = 0; i < spotLightCount; i++)
		outColor += CalcSpotLight(i, norm, viewDir);

	for(int i = 0; i < pointLightCount; i++)
		outColor += CalcPointLight(i, norm, viewDir);
		
	FragColor = vec4(outColor, 1.0);
}

vec3 CalcDirLight(int index, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLights[index].direction);
	
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 materialDiffuse = material.diffuseTexture == -1 ? material.diffuseColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.diffuseTexture)));
	vec3 materialSpecular = material.specularTexture == -1 ? material.specularColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.specularTexture)));

	vec3 ambient = dirLights[index].ambient * materialDiffuse;
	vec3 diffuse = dirLights[index].diffuse * diff * materialDiffuse;
	vec3 specular = dirLights[index].specular * spec * materialSpecular;
	
	float shadow = CalcDirLightShadow(index, normal);
	
	return (ambient + (1.0 - shadow)*(diffuse + specular));
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
	float bias = max(0.05 * (1.0 - dot(normal, -dirLights[layer].direction)), 0.005);
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

float LinearizeDepth(float depth)
{
	float minD = 0.95;
	float maxD = 1.0;

	float newDepth = (depth - minD) / (maxD-minD);
	
	return newDepth;
    
}

vec3 CalcSpotLight(int index, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(spotLights[index].position - fs_in.FragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	// attenuation
    float distance = length(spotLights[index].position - fs_in.FragPos);
    float attenuation = 1.0 / (spotLights[index].constant + spotLights[index].linear * distance + spotLights[index].quadratic * (distance * distance));
	
	// spotlight intensity
    float theta = dot(lightDir, normalize(-spotLights[index].direction)); 
    float epsilon = spotLights[index].cutOff - spotLights[index].outerCutOff;
    float intensity = clamp((theta - spotLights[index].outerCutOff) / epsilon, 0.0, 1.0);

	vec3 materialDiffuse = material.diffuseTexture == -1 ? material.diffuseColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.diffuseTexture)));
	vec3 materialSpecular = material.specularTexture == -1 ? material.specularColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.specularTexture)));
	
	// combine results
    vec3 ambient = spotLights[index].ambient * materialDiffuse;
    vec3 diffuse = spotLights[index].diffuse * diff * materialDiffuse;
    vec3 specular = spotLights[index].specular * spec * materialSpecular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
	
	float shadow = 0.0;
	vec4 fragPos_LightSpace = spotLights[index].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	shadow = CalcSpotLightShadow(fragPos_LightSpace, index, normal, lightDir, spotLights[index].farPlane);
	
	
	return (1.0 - shadow)*(ambient + diffuse + specular);
}

float CalcSpotLightShadow(vec4 fragPosLightSpace, int depthMapLayer, vec3 norm, vec3 lightDir, float FarPlane)
{
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
	float bias = max(0.05 * (1.0 - dot(norm, -lightDir)), 0.005);
	const float biasModifier = 1.0f;
	bias *= 1 / (FarPlane * biasModifier);
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(spotDepthMaps, 0));
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(spotDepthMaps, vec3(projCoords.xy + vec2(x, y) * texelSize, depthMapLayer)).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	return shadow;
}

vec3 CalcPointLight(int index, vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(pointLights[index].position - fs_in.FragPos);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	
	// attenuation
    float distance = length(pointLights[index].position - fs_in.FragPos);
    float attenuation = 1.0 / (pointLights[index].constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));

	vec3 materialDiffuse = material.diffuseTexture == -1 ? material.diffuseColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.diffuseTexture)));
	vec3 materialSpecular = material.specularTexture == -1 ? material.specularColor : vec3(texture(material.textureArray, vec3(fs_in.TexCoords, material.specularTexture)));
	
	// combine results
    vec3 ambient = pointLights[index].ambient * materialDiffuse;
    vec3 diffuse = pointLights[index].diffuse * diff * materialDiffuse;
    vec3 specular = pointLights[index].specular * spec * materialSpecular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
	float shadow = CalcPointLightShadow(index);
    return (1.0 - shadow)*(ambient + diffuse + specular);
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
	const float biasModifier = 1.0f;
	float bias = 0.005;
	bias *= 1 / (pointLights[index].farPlane * biasModifier);

	float shadow = 0.0;
	float viewDistance = length(viewPos - fs_in.FragPos);
	float diskRadius = (1.0 + (viewDistance / drawDistance)) / pointLights[index].farPlane;
	for(int i = 0; i < samples; ++i)
	{
		vec3 sampleCoord = fragToLight + gridSamplingDisk[i] * diskRadius;
		float closestDepth = texture(pointDepthMaps, vec4(sampleCoord, index)).r;
		closestDepth *= drawDistance;   // undo mapping [0;1]
		if(currentDepth - bias > closestDepth)
			shadow += 1.0;
	}
	shadow /= float(samples);

    return shadow;
}