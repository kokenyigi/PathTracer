#version 330 core

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoords;

uniform sampler2D uTexture;
uniform bool uDoWeHaveTexture = false;
uniform vec3 uColor = vec3(1,1,1);

//SpotLight Data
uniform vec3 uCameraPosition = vec3(0,0,0);


//Ambient Light Data
float ambientStrength = 0.9f;
vec3 ambientColor = vec3(0.1f,0.1f,0.1f);

//Diffuse Light Data
vec3 diffuseColor = vec3(1,1,1);


layout(location = 0) out vec4 fragcolor;
        
void main()
{
    
    //Ambiant Light Calculation
    vec3 ambientLight = ambientStrength * ambientColor ;

    vec3 fragNormal = normalize(vNormal);

    vec3 lightPos = uCameraPosition + vec3(0,uCameraPosition.x,0);
    vec3 toLight =normalize(lightPos - vPosition);
    vec3 fromLight = -toLight;
    
    //Diffuse Light Calculation
    float diffuseStrength = max(dot(fragNormal,toLight),0); 
    vec3 diffuseLight = diffuseStrength * diffuseColor;

    //Specular Light Calculation
    vec4 shadedColor = vec4(ambientLight + diffuseLight ,1.0);


    if(uDoWeHaveTexture)
    {
        shadedColor *= texture(uTexture,vTexCoords);
    }

    shadedColor *= vec4(uColor,1);

    fragcolor = shadedColor;
    
    
    return;
    
};