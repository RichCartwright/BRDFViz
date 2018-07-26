#version 330 core

layout(location = 0) in vec3 vertexPos_modelspace;

void main()
{
    gl_Position.xyz = vertexPos_modelspace;
    gl_Position.w = 1.0;
}
