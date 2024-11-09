#version 430 core
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aPredictedPosition;
layout(location = 2) in vec3 aVelocity;
layout(location = 3) in float aDensity;

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
	vs_out.PredictedPosition = aPredictedPosition;
	vs_out.Velocity = aVelocity;
	vs_out.Density = aDensity;

    mat4 finalMat = viewProj * model;

	vec3 scale3D = vec3(
        dot(finalMat[0].xyz, finalMat[0].xyz), // Length of the first column (X scale)
        dot(finalMat[1].xyz, finalMat[1].xyz), // Length of the second column (Y scale)
        dot(finalMat[2].xyz, finalMat[2].xyz)  // Length of the third column (Z scale)
    );

    // Calculate the overall scale of the object
    float scale = sqrt(min(scale3D.x, min(scale3D.y, scale3D.z)));
        
	// Assign the calculated point size to gl_PointSize
	gl_PointSize = scale * particleSize;

    // Set the final position for the vertex in clip space (view + projection)
    gl_Position = viewProj * model * vec4(aPosition, 1.0);
}