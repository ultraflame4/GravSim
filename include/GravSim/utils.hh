//
// Created by powew on 21/10/2023.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/matrix_projection.hpp>
#include "window.hh"
#define CLAMP_VEL()

const glm::mat4 IdentityMat(1.f);

const glm::vec3 VEC_UP(0, 1, 0);
const glm::vec3 VEC_DOWN(0, -1, 0);
const glm::vec3 VEC_LEFT(-1, 0, 0);
const glm::vec3 VEC_RIGHT(1, 0, 0);
const glm::vec3 VEC_FORWARD(0, 0, -1);

static glm::vec2
screen2WorldPos(glm::vec2 pos, glm::mat4 proj, glm::mat4 view, int width, int height) {
    glm::vec2 pos_ = glm::unProject(
        glm::vec3(pos.x, height - pos.y, 0),
        view,
        proj,
        glm::vec4(0, 0, width, height)
    );
    //    pos_.y = pos_.y-height;
    return pos_;
}

static glm::vec2
screen2WorldPos(glm::vec2 pos, glm::mat4 proj, glm::mat4 view, const Window& window) {
    return screen2WorldPos(pos, proj, view, window.width, window.height);
}

struct Color {
    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;
};
namespace colors {
    static const Color WHITE = Color{1.f, 1.f, 1.f};
}