#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragColor;

void main() {
    gl_Position = proj * view * model * vec4(aPos, -1, 1.0);
    fragColor = aColor;
}
