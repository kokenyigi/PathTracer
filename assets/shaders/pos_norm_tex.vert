#version 330 core
        
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

uniform mat4 projectionTransform;
uniform mat4 viewTransform;

out vec3 vPosition;
out vec3 vNormal;
out vec2 vTexCoords;
        
void main()
{
    gl_Position =  projectionTransform * viewTransform  * vec4(aPosition,1.0);

    vPosition = aPosition;
    vNormal = aNormal;
    vTexCoords = aTexCoords;
};