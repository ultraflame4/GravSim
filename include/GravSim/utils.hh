//
// Created by powew on 21/10/2023.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/ext/matrix_projection.hpp>
#include "window.hh"

const glm::mat4 IdentityMat(1.f);

const glm::vec3 up(0, 1, 0);
const glm::vec3 down(0, -1, 0);
const glm::vec3 left(-1, 0, 0);
const glm::vec3 right(1, 0, 0);
const glm::vec3 forward(0, 0, -1);


static glm::vec2 screen2WorldPos(glm::vec2 pos, glm::mat4 proj, int width, int height) {
    glm::vec2 pos_ = glm::unProject(glm::vec3(pos.x, pos.y, 0), IdentityMat, proj,
                                    glm::vec4(0, 0, width, height));
    pos_.y = -pos_.y;
    return pos_;
}

static glm::vec2 screen2WorldPos(glm::vec2 pos, glm::mat4 proj, const Window &window) {
    return screen2WorldPos(pos,proj, window.width, window.height);
}