#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in float aRadius;
layout (location = 2) in vec3 aColor;

uniform vec2 viewport;
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


out vec3 vColor;
out float pRadius;
out vec2 pointPos;

void main()
{
    //    gl_PointSize = floatBitsToInt(aRadius);

    pRadius = aRadius;
//    pRadius = aRadius;
    gl_PointSize = pRadius*2;
//    gl_Position = vec4(aPos, 0, 1.0);
//    gl_Position =  vec4(aPos, 0, 1.0);
    gl_Position = proj * view * model * vec4(aPos, -1, 1.0);
    vColor = aColor;

    vec2 ndcPos = gl_Position.xy / gl_Position.w;

    pointPos    = viewport * (ndcPos*0.5 + 0.5);
}