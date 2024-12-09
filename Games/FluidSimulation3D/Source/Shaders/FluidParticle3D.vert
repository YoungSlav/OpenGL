#version 430 core

struct Particle
{
    vec3 Position;
    vec3 PredictedPosition;
    vec3 Velocity;
    float Density;
    float iDensity;
};

layout(std430, binding = 0) buffer ParticlesBuffer
{
	Particle Particles[];
};

uniform mat4 model;
uniform mat4 viewProj;
uniform float particleSize;

out VS_OUT {
	vec3 PredictedPosition;
	vec3 Velocity;
	float Density;
} vs_out;

void main()
{
	vs_out.PredictedPosition = Particles[gl_VertexID].PredictedPosition;
	vs_out.Velocity = Particles[gl_VertexID].Velocity;
	vs_out.Density = Particles[gl_VertexID].Density;

    mat4 finalMat = viewProj * model;

	vec3 scale3D = vec3(
        dot(finalMat[0].xyz, finalMat[0].xyz), // Length of the first column (X scale)
        dot(finalMat[1].xyz, finalMat[1].xyz), // Length of the second column (Y scale)
        dot(finalMat[2].xyz, finalMat[2].xyz)  // Length of the third column (Z scale)
    );

    // Calculate the overall scale of the object
    float scale = sqrt(max(scale3D.x, max(scale3D.y, scale3D.z)));
        
	// Assign the calculated point size to gl_PointSize
	gl_PointSize = scale * particleSize;
    
    // Set the final position for the vertex in clip space (view + projection)
    gl_Position = viewProj * model * vec4(Particles[gl_VertexID].Position, 1.0);
}