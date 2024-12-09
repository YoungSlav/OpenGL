#version 430 core


in VS_OUT {
	vec3 PredictedPosition;
	vec3 Velocity;
	float Density;
} fs_in;

out vec4 FragColor;

uniform float maxSpeed;

uniform float TargetDensity;

const vec3 Green = vec3(0.0f, 1.0f, 0.0f);  // Low density
const vec3 Blue = vec3(0.0f, 0.0f, 1.0f);   // Good density
const vec3 Red = vec3(1.0f, 0.0f, 0.0f);    // High density

uniform vec3 Cold;
uniform vec3 Middle;
uniform vec3 Hot;

void main()
{
    //float speed = length(fs_in.Velocity);
    //const float midSpeed = maxSpeed*0.5f;
    //vec3 Color;
    //if ( speed < midSpeed )
    //    Color = mix(Cold, Middle, speed / midSpeed);
    //else
    //    Color = mix(Middle, Hot, (speed-midSpeed) / (maxSpeed-midSpeed));

    vec3 Color;
    const float MaxDensity = TargetDensity * 2.0f;
    if ( fs_in.Density < TargetDensity )
        Color = mix(Green, Blue, fs_in.Density / TargetDensity);
    else
        Color = mix(Blue, Red, (fs_in.Density-TargetDensity) / (MaxDensity-TargetDensity));

    
    FragColor = vec4(Color, 1.0);
}