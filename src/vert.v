#version 330 core

in vec3 position;
layout(location = 1) in vec3 inColour;

out vec3 outColour;

void main()
{
    gl_Position.xyz = vertexPos_modelspace;
    gl_Position.w = 1.0;
    outColour = inColour;
}
