#version 330 core
out vec4 FragColor;
uniform vec2[254] bodies_pos;
uniform float feathering;

in vec4 gl_FragCoord;
in vec3 vColor;
in float pRadius;
in vec2 pointPos;


void main()
{
    float dist = distance(pointPos, gl_FragCoord.xy);
//    if (dist > pRadius) discard;

    float featherStart = pRadius - feathering;
    float featherVal = dist - featherStart;
    float normalised = 1 - (featherVal / feathering);
    float alpha = clamp(normalised, 0, 1);

    FragColor = vec4(vColor, alpha);


}