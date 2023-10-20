#version 330 core
out vec4 FragColor;
uniform vec2[254] bodies_pos;

uniform vec4 viewport;
//uniform float[] bodies_r;

in vec4 gl_FragCoord;
in vec3 vColor;

void main()
{
//
//    vec4 ndcPos = vec4(
//    (gl_FragCoord.x / viewport.b - 0.5) * 2.0,
//    (gl_FragCoord.y / viewport.a - 0.5) * 2.0,
//    (gl_FragCoord.z - 0.5) * 2.0,
//    1.0);
//
//
//    vec2 position = ndcPos.xy;
//
//    float nearest_dist = 1000;
//    for (int i=0;i<bodies_pos.length();++i)
//    {
//        vec2 bodyPos = bodies_pos[i];
//        float current_dist = distance(bodyPos, position);
//        if (nearest_dist > current_dist){
//            nearest_dist = current_dist;
//        }
//    }
    FragColor = vec4(vColor, 1.0);
//    if (nearest_dist < 0.1){
//        FragColor = vec4(1f,1f,1f,1f);
//    }


//    FragColor = ndcPos;


}