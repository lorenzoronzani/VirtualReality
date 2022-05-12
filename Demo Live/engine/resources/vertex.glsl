#version 440 core



uniform mat4 projection;
uniform mat4 modelview;
uniform mat3 normalMatrix;
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;
layout (location = 2) in vec2 aTexCoord;
// Varying:
out vec4 fragPosition;
out vec3 normal; 
out vec2 TexCoord;
out mat4 model_view;


void main(void)
{
    model_view=modelview;
    fragPosition = modelview * vec4(in_Position, 1.0f);
    gl_Position = projection * fragPosition; 
    normal = normalMatrix * in_Normal;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
}