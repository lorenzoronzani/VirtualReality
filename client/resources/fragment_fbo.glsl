#version 440 core
   
in vec2 texCoord;
   
out vec4 fragOutput;   

// Texture mapping:
layout(binding = 0) uniform sampler2D texSampler;

void main(void)   
{  
    // Texture element:
    vec4 texel = texture(texSampler, texCoord);      
      
    // Final color:
    fragOutput = texel;       
}