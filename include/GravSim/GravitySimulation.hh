//
// Created by powew on 20/10/2023.
//

#pragma once

#include <glm/glm.hpp>
#include "shader.hh"
#include "VertexObject.hh"
#include "window.hh"
#include "Line.hh"

struct GravBodyVertex {
    float x;
    float y;
    float radius;
    float r;
    float g;
    float b;
};
static_assert(sizeof(GravBodyVertex) == sizeof(float) * 6, "ERROR GravBodyVertex struct contains padding!");

enum BodyActiveStatus : uint8_t {
    ACTIVE,
    NO_COLLIDE,
    DISABLED
};

struct GravBodyPhysical {
    glm::vec2 pos;
    glm::vec2 last_pos;
    glm::vec2 vel;
    float mass;
    float radius;
    bool active;
    bool phantom;
    int index;

    void Accelerate(glm::vec2 direction, float forceAmt) {
        vel += direction * (forceAmt / mass);
    }

    void addImmediateForce(glm::vec2 direction, float forceAmt) {
        vel = direction * (forceAmt / mass);
    }
};

class GravitySimulation {
public:
    std::vector<GravBodyVertex> bodies;
    std::vector<GravBodyPhysical> physicalBodies;
    Shader shader;
    std::mutex bodies_mutex;

    int feathering_loc;
    int viewport_loc;
    int model_loc;
    int view_loc;
    int proj_loc;

    float gravityConstant = 500.f;
    VertexObject *vo;
    Window &window;

    GravitySimulation(Window &window);

    bool gravity = true;
    bool collision = true;
    bool debug = false;

    void clear();
    void load();

    void update();

    /**
     * Resolve 2 physical bodies with the same position
     * @param a
     * @param b
     */
    void resolveFused(GravBodyPhysical &a, GravBodyPhysical &b);

    void draw(glm::mat4 view, glm::mat4 proj);

    GravBodyPhysical &AddBody(float x, float y, float radius, float mass, float color[3], bool active = true);

private:
    std::shared_ptr<spdlog::logger> logger = logging::get<GravitySimulation>();
    void UpdateGravBodyPhysics(GravBodyPhysical &bodyp, int index);

    void ApplyCollisionForces(GravBodyPhysical &bodyp, GravBodyPhysical &otherp);

    void ApplyGravityForce(GravBodyPhysical &bodyp, GravBodyPhysical &otherp);

    std::vector<Line> debugLines;
    void drawDebugLines(glm::mat4 view, glm::mat4 proj);

    GravBodyVertex & vertex(GravBodyPhysical &bodyp);
};