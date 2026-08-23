#version 430 core

layout(location = 0) in vec3 aPosition;


struct TlasBvhNode
{
    vec4 boundingBoxMin;
    vec4 boundingBoxMax;

    int minChildIndex;
    int maxChildIndex;
    int p0;
    int p1;
};

layout(std430, binding = 0) readonly buffer tlasBvhBoxes
{
    TlasBvhNode nodes[];
};

uniform mat4 uViewProjection;

void main()
{
    TlasBvhNode node = nodes[gl_InstanceID];

    vec3 boxCenter = (node.boundingBoxMin.xyz + node.boundingBoxMax.xyz)* 0.5;
    vec3 boxSize = node.boundingBoxMax.xyz - node.boundingBoxMin.xyz;

    vec3 localPosition = boxCenter + aPosition * boxSize;

    gl_Position = uViewProjection  * vec4(localPosition, 1.0);
}