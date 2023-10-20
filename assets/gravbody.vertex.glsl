#version 330 core
layout (location = 0) in vec2 aPos;

out vec3 bgColor;

void main()
{
    gl_PointSize = 10.0;
    gl_Position = vec4(aPos, 0, 1.0);
    bgColor = vec3(0.1,0.1,0.1);
}