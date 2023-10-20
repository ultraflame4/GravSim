#version 330 core
out vec4 FragColor;
uniform vec2[254] bodies_pos;


in vec4 gl_FragCoord;
in vec3 vColor;
in float pRadius;
in vec2 pointPos;

void main()
{
    float dist = distance(pointPos, gl_FragCoord.xy);
    if (dist > pRadius) discard;
    FragColor = vec4(vColor, 1.0);


}