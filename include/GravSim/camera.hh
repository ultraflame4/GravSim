#pragma once
#include <algorithm>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include "GravSim/utils.hh"

class Camera {
  public:
    int viewport_w = 1;
    int viewport_h = 1;

    glm::vec3 position        = glm::vec3(0.f, 0.f, 0.f);
    glm::vec3 target_position = glm::vec3(0.f, 0.f, 0.f);
    glm::mat4 view            = glm::mat4(1.0f);
    glm::mat4 proj;

    float current_zoom   = 1.f;
    float target_zoom    = 1.f;
    float zoom_speed     = .1f;
    const float max_zoom = 5;
    const float min_zoom = .2f;

    Camera() {}

    /**
     * @brief Changes the camera zoom by a specific amount
     *
     */
    void change_zoom(float amt) { target_zoom -= amt * zoom_speed; }
    glm::vec2 screen2World(glm::vec2 pos) {
        return screen2WorldPos(pos, proj, view, viewport_w, viewport_h);
    }

    void resize_viewport(int viewport_w, int viewport_h) {
        this->viewport_w = viewport_w;
        this->viewport_h = viewport_h;
        proj             = createOrtho(current_zoom);
    }

    void update(float dt) {
        target_zoom = std::clamp(target_zoom, min_zoom, max_zoom);

        if (abs(current_zoom - target_zoom) < 0.01f) {
            // Ignore miniscule distance by teleporting.
            current_zoom = target_zoom;
        } else {
            current_zoom = std::lerp(current_zoom, target_zoom, 10 * dt);
            proj         = createOrtho(current_zoom);
        }

        if (abs(length(position - target_position)) < (0.01f * current_zoom)) {
            position = glm::mix(position, target_position, 10 * dt);
        } else {
            position = glm::mix(position, target_position, 10 * dt);
        }
        view = glm::lookAt(position, position + VEC_FORWARD, VEC_UP);
    }

  private:
    glm::mat4 createOrtho(float size, float zNear = .1f, float zFar = 100.0f) {
        float halfW = size * viewport_w;
        float halfH = size * viewport_h;
        return glm::ortho(-halfW, halfW, -halfH, halfH, zNear, zFar);
    }
};