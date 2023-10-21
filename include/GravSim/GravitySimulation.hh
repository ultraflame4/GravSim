//
// Created by powew on 20/10/2023.
//

#pragma once

#include <glm/glm.hpp>
#include "shader.hh"
#include "VertexObject.hh"
#include "window.hh"

struct GravBodyVertex {
    float x;
    float y;
    float radius;
    float r;
    float g;
    float b;
};
static_assert(sizeof(GravBodyVertex) == sizeof(float) * 6, "ERROR GravBodyVertex struct contains padding!");

struct GravBodyPhysical {
    bool active;
    glm::vec2 pos;
    glm::vec2 last_pos;
    glm::vec2 vel;
    glm::vec2 last_vel;
    float mass;
    float radius;
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


    int feathering_loc;
    int viewport_loc;
    int model_loc;
    int view_loc;
    int proj_loc;

    float gravityConstant = 50.f;
    VertexObject *vo;
    Window &window;

    GravitySimulation(Window &window);

    bool gravity = true;
    bool collision = true;

    void load();

    void update();

    void draw(glm::mat4 view, glm::mat4 proj);

    GravBodyPhysical &AddBody(float x, float y, float radius, float mass, float color[3], bool active = true);

private:
    void UpdateGravBodyPhysics(GravBodyPhysical &bodyp, int index);

    void ApplyCollisionForces(GravBodyPhysical &bodyp, GravBodyPhysical &otherp);

    void ApplyGravityForce(GravBodyPhysical &bodyp, GravBodyPhysical &otherp);


};