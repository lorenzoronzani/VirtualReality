#version 440 core

uniform mat4 projection;
uniform mat4 modelview;

layout(location = 0) in vec3 in_Position;      

out vec3 texCoord;

void main(void)
{
    texCoord = in_Position;
    vec4 positi=projection * modelview * vec4(in_Position, 1.0f);
    gl_Position = vec4(positi.x,positi.y,positi.w, positi.w);            
}