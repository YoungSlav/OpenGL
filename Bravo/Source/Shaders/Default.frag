#version 410 core

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D height;	//TODO
	sampler2D ambient;	//TODO
	sampler2D normal;

	float shininess;
};

struct LightColor
{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct DirectionalLight
{
	vec3 direction;

	LightColor light;

	sampler2DArray shadowMap;
	float cascadePlaneDistances[16];
	mat4 lightSpaceMatrices[16];
	int cascadeCount;

};

struct PointLight
{
	LightColor light;

	vec3 position;

	float constant;
	float linear;
	float quadratic;

	//sampler2D shadowMap;
	//mat4 lightSpaceMatrix;
};

struct SpotLight
{
	LightColor light;

	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
  
	float constant;
	float linear;
	float quadratic;

	sampler2D shadowMap;
	mat4 lightSpaceMatrix;
};

#define MAX_POINT_LIGHTS 20
#define MAX_SPOT_LIGHTS 20



in VS_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoords;
} fs_in;
  
uniform mat4 view;
uniform vec3 viewPos;

uniform DirectionalLight dirLight;

uniform PointLight pointLights[MAX_POINT_LIGHTS];
uniform int pointLightsNum;

uniform SpotLight spotLights[MAX_SPOT_LIGHTS];
uniform int spotLightsNum;

uniform Material material;

out vec4 FragColor;


// function prototypes
vec3 CalcDirLight(vec3 normal, vec3 viewDir);
float CalcDirLightShadow(vec3 norm);

vec3 CalcPointLight(int index, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(int index, vec3 normal, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 norm, vec3 lightDir);

void main()
{    
	// properties
	vec3 norm;
	vec3 TexturenNorm = texture(material.normal, fs_in.TexCoords).rgb;
	if ( TexturenNorm == vec3(0) )
	{
		norm = fs_in.TBN[2];
	}
	else
	{
		norm = normalize(fs_in.TBN * normalize((TexturenNorm)*2.0 - 1.0));
	}

	vec3 viewDir = normalize(viewPos - fs_in.FragPos);
    
	// directional lighting
	vec3 outColor = CalcDirLight(norm, viewDir);
	//// point lights
	//for(int i = 0; i < pointLightsNum; i++)
	//	outColor += CalcPointLight(i, norm, viewDir);
	// spot light
	for(int i = 0; i < spotLightsNum; i++)
		outColor += CalcSpotLight(i, norm, viewDir);    
		
	FragColor = vec4(outColor, 1.0);
}

// calculates the color when using a directional light.
vec3 CalcDirLight(vec3 normal, vec3 viewDir)
{
	vec3 lightDir = normalize(-dirLight.direction);
	// diffuse shading
	float diff = max(dot(normal, lightDir), 0.0);
	// specular shading
	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = dirLight.light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 diffuse = dirLight.light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 specular = dirLight.light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
	
	float shadow = CalcDirLightShadow(normal);
	
	return (ambient + (1.0 - shadow)*(diffuse + specular));
}

float CalcDirLightShadow(vec3 norm)
{
	// select cascade layer
	vec4 fragPosViewSpace = view * vec4(fs_in.FragPos, 1.0);
	float depthValue = abs(fragPosViewSpace.z);

	int layer = -1;
	for (int i = 0; i < dirLight.cascadeCount; ++i)
	{
		if (depthValue < dirLight.cascadePlaneDistances[i])
		{
			layer = i;
			break;
		}
	}
	if (layer == -1)
	{
		layer = dirLight.cascadeCount-1;
	}

	vec4 fragPosLightSpace = dirLight.lightSpaceMatrices[layer] * vec4(fs_in.FragPos, 1.0);
	// perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	// get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0)
    {
        return 0.0;
    }
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(norm, -dirLight.direction)), 0.005);
	const float biasModifier = 1.0f;
	bias *= 1 / (dirLight.cascadePlaneDistances[layer] * biasModifier);
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / vec2(textureSize(dirLight.shadowMap, 0));
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(dirLight.shadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
			shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
		}    
	}
	shadow /= 9.0;
        
	return shadow;
}

// calculates the color when using a point light.
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

	// combine results
    vec3 ambient = pointLights[index].light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = pointLights[index].light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = pointLights[index].light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
	ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
	
	float shadow = 0.0;
	//vec4 fragPos_LightSpace = pointLights[index].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	//shadow = ShadowCalculation(fragPos_LightSpace, pointLights[index].shadowMap, normal, lightDir);

	return (ambient + (1.0 - shadow)*(diffuse + specular));
}

// calculates the color when using a spot light.
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

	// combine results
    vec3 ambient = spotLights[index].light.ambient * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 diffuse = spotLights[index].light.diffuse * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
    vec3 specular = spotLights[index].light.specular * spec * vec3(texture(material.specular, fs_in.TexCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;


	float shadow = 0.0;
	vec4 fragPos_LightSpace = spotLights[index].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	shadow = ShadowCalculation(fragPos_LightSpace, spotLights[index].shadowMap, normal, lightDir);


	return (ambient + (1.0 - shadow)*(diffuse + specular));
}


float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 norm, vec3 lightDir)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r; 
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;
	// calculate bias (based on depth map resolution and slope)
	float bias = max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	// check whether current frag pos is in shadow
	// float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
	// PCF
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
    
	// keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
	if(projCoords.z > 1.0)
		shadow = 0.0;
        
	return shadow;
}