#version 330 core

#define NR_POINT_LIGHTS 4
#define NR_SPOT_LIGHTS 4

struct Material
{
	sampler2D diffuse;
	sampler2D specular;
	sampler2D height;
	sampler2D ambient;
	sampler2D normal;

	float shininess;
};

struct DirLight
{
	vec3 direction;
	vec3 lightColor;
	sampler2D shadowMap;
	mat4 lightSpaceMatrix;
};

struct PointLight
{
	vec3 position;
	float constant;
	float linear;
	float quadratic;
	vec3 lightColor;
};

struct SpotLight
{
	vec3 position;
	vec3 direction;
	float cutOff;
	float outerCutOff;
	float constant;
	float linear;
	float quadratic;
	vec3 lightColor;
	sampler2D shadowMap;
	mat4 lightSpaceMatrix;
};



in VS_OUT {
	vec3 FragPos;
	mat3 TBN;
	vec2 TexCoords;
} fs_in;
  
uniform vec3 viewPos;

uniform DirLight dirLight;

uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform int pointLightsNum;

uniform SpotLight spotLights[NR_SPOT_LIGHTS];
uniform int spotLightsNum;

uniform Material material;

out vec4 FragColor;


// function prototypes
vec3 CalcDirLight(vec3 normal, vec3 viewDir);
vec3 CalcPointLight(int index, vec3 normal, vec3 viewDir);
vec3 CalcSpotLight(int index, vec3 normal, vec3 viewDir);
float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 norm, vec3 lightDir);

void main()
{    
	// properties
	vec3 norm;
	vec3 TexturenNorm = texture(material.normal, fs_in.TexCoords).rgb;
	TexturenNorm.y = 1.0-TexturenNorm.y;
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
	vec3 result = CalcDirLight(norm, viewDir);
	// point lights
	for(int i = 0; i < pointLightsNum; i++)
		result += CalcPointLight(i, norm, viewDir);
	// spot light
	for(int i = 0; i < spotLightsNum; i++)
		result += CalcSpotLight(i, norm, viewDir);    
		
	FragColor = vec4(result, 1.0);
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

	vec3 ambientColor = vec3(dirLight.lightColor * 0.05);
	vec3 diffuseColor = vec3(dirLight.lightColor * 0.5);
	vec3 specularColor = vec3(dirLight.lightColor * 0.1);

	// combine results
	vec3 ambient = ambientColor * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 diffuse = diffuseColor * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 specular = specularColor * spec * vec3(texture(material.specular, fs_in.TexCoords));
	
	vec4 fragPos_LightSpace = dirLight.lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	float shadow = ShadowCalculation(fragPos_LightSpace, dirLight.shadowMap, normal, lightDir);

	return (ambient + (1.0 - shadow)*(diffuse + specular));
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

	vec3 ambientColor = vec3(pointLights[index].lightColor * 0.05);
	vec3 diffuseColor = vec3(pointLights[index].lightColor * 0.8);
	vec3 specularColor = vec3(pointLights[index].lightColor * 1.0);

	// combine results
	vec3 ambient = ambientColor * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 diffuse = diffuseColor * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 specular = specularColor * spec * vec3(texture(material.specular, fs_in.TexCoords));
	
	// attenuation
	float distance = length(pointLights[index].position - fs_in.FragPos);
	float attenuation = 1.0 / (pointLights[index].constant + pointLights[index].linear * distance + pointLights[index].quadratic * (distance * distance));
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

	vec3 ambientColor = vec3(spotLights[index].lightColor * 0.0);
	vec3 diffuseColor = vec3(spotLights[index].lightColor * 1.0);
	vec3 specularColor = vec3(spotLights[index].lightColor * 1.0);

	// combine results
	vec3 ambient = ambientColor * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 diffuse = diffuseColor * diff * vec3(texture(material.diffuse, fs_in.TexCoords));
	vec3 specular = specularColor * spec * vec3(texture(material.specular, fs_in.TexCoords));
	

	// attenuation
	float distance = length(spotLights[index].position - fs_in.FragPos);
	float attenuation = 1.0 / (spotLights[index].constant + spotLights[index].linear * distance + spotLights[index].quadratic * (distance * distance));
	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	// spotlight intensity
	float theta = dot(lightDir, normalize(-spotLights[index].direction)); 
	float epsilon = spotLights[index].cutOff - spotLights[index].outerCutOff;
	float intensity = clamp((theta - spotLights[index].outerCutOff) / epsilon, 0.0, 1.0);
	ambient *= intensity;
	diffuse *= intensity;
	specular *= intensity;

	vec4 fragPos_LightSpace = spotLights[index].lightSpaceMatrix * vec4(fs_in.FragPos, 1.0);
	float shadow = ShadowCalculation(fragPos_LightSpace, spotLights[index].shadowMap, normal, lightDir);


	return (ambient + (1.0 - shadow)*(diffuse + specular));
}

float ShadowCalculation(vec4 fragPosLightSpace, sampler2D shadowMap, vec3 norm, vec3 lightDir)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
	if(projCoords.z > 1.0)
		return 0.0;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow
	float bias = 0.0;//max(0.05 * (1.0 - dot(norm, lightDir)), 0.005);
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -1; x <= 1; ++x)
	{
		for(int y = -1; y <= 1; ++y)
		{
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	shadow /= 9.0;
	//shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;

    return shadow;
}
