#version 330 core
layout (location = 0) in vec2 aPos;

out vec3 backgroundColor;

void main()
{
    gl_Position = vec4(aPos, 0, 1.0);
    backgroundColor = vec3(0,0,0);
}