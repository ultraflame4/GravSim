#version 330 core

in vec3 fragColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec4 FragColor;
void main() {
    FragColor = vec4(fragColor,1.f);
}
