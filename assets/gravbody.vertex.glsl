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
    vec4 a =  proj * model * vec4(1 + aRadius,0,0,1.0);
    vec4 b =  proj * model * vec4(1,0,0,1.0);

    //    gl_Position = vec4(aPos, 0, 1.0);
//    gl_Position =  vec4(aPos, 0, 1.0);
    gl_Position = proj * view * model * vec4(aPos, -1, 1.0);
    vColor = aColor;

    vec2 ndcPos = gl_Position.xy / gl_Position.w;

    pointPos    = viewport * (ndcPos*0.5 + 0.5);

    // Scale point size -> Referenced from https://stackoverflow.com/a/25783231
    gl_PointSize = viewport.y * proj[1][1] * aRadius / gl_Position.w;
    pRadius = gl_PointSize/2;

}