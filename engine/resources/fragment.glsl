#version 440 core

#define MAX_LIGHTS 16
out vec4 frag_Output;
in vec4 fragPosition;
in vec3 normal;   
in vec2 TexCoord;   
in mat4 model_view;
// Material properties:
uniform vec3 matEmission;
uniform vec3 matAmbient;
uniform vec3 matDiffuse;
uniform vec3 matSpecular;
uniform float matShininess;
// Light properties:
struct omniLight{
    vec3 lightPosition; 
    vec3 lightAmbient; 
    vec3 lightDiffuse; 
    vec3 lightSpecular;
};
struct SpotLight {
vec3 direction;
float cutOff;
float outerCutOff;
  
vec3 lightPosition; 
vec3 lightAmbient; 
vec3 lightDiffuse; 
vec3 lightSpecular;    
};

uniform omniLight lights[MAX_LIGHTS];
uniform SpotLight lightsSpot[MAX_LIGHTS];

uniform int numLights;
uniform int numLightsSpot;

layout(binding = 0) uniform sampler2D texture1;

void main(void)
{
    vec3 finalColor=vec3(0.0,0.0,0.0);
    for(int i=0;i<numLights;i++){

        // Ambient term:
        vec3 fragColor = matEmission + matAmbient * lights[i].lightAmbient;
        // Diffuse term:
        vec3 _normal = normalize(normal);
        vec3 lightDirection = normalize(lights[i].lightPosition - fragPosition.xyz);      
        float nDotL = dot(lightDirection, _normal);   
        if (nDotL > 0.0f)
        {
            fragColor += matDiffuse * nDotL * lights[i].lightDiffuse;
      
            // Specular term:
            vec3 halfVector = normalize(lightDirection + normalize(-fragPosition.xyz));                     
            float nDotHV = dot(_normal, halfVector);         
            fragColor += matSpecular * pow(nDotHV, matShininess) * lights[i].lightSpecular;
        } 
        finalColor=finalColor+fragColor;
    }
    for(int i=0;i<numLightsSpot;i++){
        // Ambient term:
        vec3 fragColor = matEmission + matAmbient * lightsSpot[i].lightAmbient;
        // Diffuse term:
        vec3 _normal = normalize(normal);
        vec3 lightDirection = normalize(normalize(lightsSpot[i].lightPosition - fragPosition.xyz)*mat3(model_view));   
        float theta = dot(lightDirection, normalize(-lightsSpot[i].direction));    
        float nDotL = dot(lightDirection, _normal);   
        if (nDotL > 0 && theta > lightsSpot[i].cutOff)
        {
            fragColor += matDiffuse * nDotL * lightsSpot[i].lightDiffuse;
            // Specular term:
            vec3 halfVector = normalize(lightDirection + normalize((-fragPosition.xyz)));                     
            float nDotHV = dot(_normal, halfVector);         
            fragColor += matSpecular * pow(nDotHV, matShininess) * lightsSpot[i].lightSpecular;
        } 
          
        finalColor=finalColor+fragColor;
    }
    frag_Output = texture(texture1, TexCoord)*vec4(finalColor, 1.0f);
}