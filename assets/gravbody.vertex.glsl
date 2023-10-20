#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aRadius;
layout (location = 2) in vec3 aColor;

out vec3 vColor;

void main()
{
//    gl_PointSize = floatBitsToInt(aRadius);
    gl_PointSize = aRadius;
    gl_Position = vec4(aPos, 0, 1.0);
    vColor = aColor;
}