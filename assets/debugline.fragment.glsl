#version 330 core

in vec3 fragColor;
out vec4 FragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main() {
    FragColor = vec4(fragColor,1.f);
}
