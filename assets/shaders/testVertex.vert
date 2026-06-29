#version 330 core

layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormals;
layout(location = 2) in vec2 aTextureCoordinates;

uniform mat4 uViewProjectionTransform;

void main()
{
    gl_Position = uViewProjectionTransform * vec4(aPosition,1.0);
}