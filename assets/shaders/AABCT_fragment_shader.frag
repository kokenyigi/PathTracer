#version 330 core

in vec2 vTextureCoordinates;

layout(location = 0) out vec4 fragColor;

uniform sampler2D uTexture;
uniform vec3 uColor = vec3(0.7,0,0.3);

void main()
{
	fragColor = vec4(uColor,1) * texture(uTexture,vTextureCoordinates);
}