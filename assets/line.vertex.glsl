#version 330 core
layout (location = 0) in vec2 aPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() {
    gl_Position = proj * view * model * vec4(aPos, -1, 1.0);
}
