#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec2 aRadius;
layout (location = 2) in vec3 aColor;

out vec3 bgColor;

void main()
{
    gl_PointSize = aRadius;
    gl_Position = vec4(aPos, 0, 1.0);
    bgColor = vec3(0.1,0.1,0.1);
}