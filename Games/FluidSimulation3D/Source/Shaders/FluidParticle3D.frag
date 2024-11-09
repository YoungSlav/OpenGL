#version 430 core


in VS_OUT {
	vec3 PredictedPosition;
	vec3 Velocity;
	float Density;
} fs_in;

out vec4 FragColor;

uniform float maxSpeed;


uniform vec3 Cold;
uniform vec3 Middle;
uniform vec3 Hot;

void main()
{
    float speed = length(fs_in.Velocity);
    const float midSpeed = maxSpeed*0.5f;
    vec3 Color;
    if ( speed < midSpeed )
        Color = mix(Cold, Middle, speed / midSpeed);
    else
        Color = mix(Middle, Hot, (speed-midSpeed) / (maxSpeed-midSpeed));

    
    FragColor = vec4(Color, 1.0);
}