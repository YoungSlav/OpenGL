#version 430 core

in vec3 vs_out;

out vec4 FragColor;

uniform vec3 corners[8];

uniform vec3 EdgeColor;
uniform float EdgeSize;

int boxEdges[24] = int[]
(
    // Front face edges
    0, 1,  // Edge between vertices 0 and 1
    1, 2,  // Edge between vertices 1 and 2
    2, 3,  // Edge between vertices 2 and 3
    3, 0,  // Edge between vertices 3 and 0

    // Back face edges
    4, 5,  // Edge between vertices 4 and 5
    5, 6,  // Edge between vertices 5 and 6
    6, 7,  // Edge between vertices 6 and 7
    7, 4,  // Edge between vertices 7 and 4

    // Connecting edges between front and back faces
    0, 4,  // Edge between vertices 0 and 4
    1, 5,  // Edge between vertices 1 and 5
    2, 6,  // Edge between vertices 2 and 6
    3, 7   // Edge between vertices 3 and 7
);

vec2 distancePointToLine2(vec3 testPoint, vec3 linePointA, vec3 linePointB);

void main()
{
    bool bDiscard = true;
    for ( int i = 0; i < 12; ++i )
    {
        vec2 dist = distancePointToLine2(vs_out, corners[boxEdges[i*2]], corners[boxEdges[i*2 + 1]]);
        if ( dist.x <= EdgeSize*EdgeSize )
        {
            bDiscard = false;
            break;
        }
    }
    if ( bDiscard )
    {
        discard;
    }

    if (!gl_FrontFacing)
    {
        FragColor = vec4(EdgeColor, 1.0);
    }
    else
    {
        FragColor = vec4(EdgeColor*0.3, 1.0);
    }
}

vec2 distancePointToLine2(vec3 testPoint, vec3 linePointA, vec3 linePointB)
{
    vec3 lineDir = normalize(linePointB - linePointA); // Direction of the line
    vec3 pointToLineA = testPoint - linePointA;        // Vector from linePointA to testPoint
    
    // Project pointToLineA onto the line direction
    float t = dot(pointToLineA, lineDir);
    vec3 projection = linePointA + t * lineDir;        // Closest point on the line to testPoint
    
    // Return the distance from the testPoint to the projection
    vec3 diff = testPoint - projection;
    float dist2 = dot(diff, diff);


    return vec2(dist2, t);
}