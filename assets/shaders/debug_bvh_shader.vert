#version 460 core

layout(location = 0) in vec3 aPosition;

struct AABB4
{
    vec4 minPoint;
    vec4 maxPoint;
};

layout(std430, binding = 0) readonly buffer BvhBoxesBuffer
{
    AABB4 boxes[];
};

layout(std430, binding = 1) readonly buffer WorldTransformsBuffer
{
    mat4 worldTransforms[];
};

uniform mat4 uViewProjection;

void main()
{
    uint bvhBoxIndex = gl_BaseInstance + uint(gl_InstanceID);

    AABB4 bvhBox = boxes[bvhBoxIndex];

    vec3 boxCenter = (bvhBox.minPoint.xyz + bvhBox.maxPoint.xyz)* 0.5;
    vec3 boxSize = bvhBox.maxPoint.xyz - bvhBox.minPoint.xyz;

    vec3 localPosition = boxCenter + aPosition * boxSize;

    mat4 worldTransform = worldTransforms[gl_DrawID];

    gl_Position = uViewProjection * worldTransform * vec4(localPosition, 1.0);
}