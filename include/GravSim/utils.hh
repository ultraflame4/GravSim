//
// Created by powew on 21/10/2023.
//

#pragma once

#include <glm/glm.hpp>
#include <glm/ext/matrix_projection.hpp>
#include "window.hh"

const glm::mat4 IdentityMat(1.f);


glm::vec2 screen2WorldPos(glm::vec2 pos,  glm::mat4 proj, int width, int height) {
    glm::vec2 pos_ = glm::unProject(glm::vec3(pos.x, pos.y, 0), IdentityMat, proj,
                                    glm::vec4(0, 0, width, height));
    pos_.y = -pos_.y;
    return pos_;
}

glm::vec2 screen2WorldPos(glm::vec2 pos, glm::mat4 proj, const Window &window) {
    return screen2WorldPos(pos,proj, window.width, window.height);
}