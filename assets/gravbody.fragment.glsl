
#version 330 core
out vec4 FragColor;
uniform vec2[] bodies_pos;
uniform vec4 viewport;
//uniform float[] bodies_r;

in vec4 gl_FragCoord ;
in vec3 bgColor;

void main()
{

    vec4 ndcPos = vec4(
    (gl_FragCoord.x / viewport.b - 0.5) * 2.0,
    (gl_FragCoord.y / viewport.a - 0.5) * 2.0,
    (gl_FragCoord.z - 0.5) * 2.0,
    1.0);

    FragColor = vec4(bgColor, 1.0);
    FragColor = ndcPos;

}