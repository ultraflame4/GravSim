#pragma once
#include <algorithm>
#include <execution>
#include "GravSim/utils.hh"
#include <glm/fwd.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

struct SimulatedBody {
    int index;
    Color color;
};

struct SimulatedPhysicsBody {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;
    std::shared_ptr<SimulatedBody> data;

    void accelerate(glm::vec2 direction, float forceAmt) {
        vel += direction * (forceAmt / mass);
    }

    void addImmediateForce(glm::vec2 direction, float forceAmt) {
        vel = direction * (forceAmt / mass);
    }
};

class Simulation {
  public:
    std::vector<SimulatedPhysicsBody> bodies;
    float stepSize        = .01f;
    float gravityConstant = 100.f;
    bool enableCollision  = true;
    bool enableGravity    = true;

  public:
    Simulation() {}

    SimulatedPhysicsBody& spawnBody(glm::vec2 xy, float radius, float mass, Color color) {
        int index = bodies.size();

        return this->bodies.emplace_back(
            SimulatedPhysicsBody{
                xy,
                glm::vec2(0, 0),
                mass,
                radius,
                std::make_shared<SimulatedBody>(SimulatedBody{index, color})
            }
        );
    }

    void step() {
        std::for_each(
            std::execution::par_unseq,
            bodies.begin(),
            bodies.end(),
            [this](SimulatedPhysicsBody& bodyp) { updateBody(bodyp, bodyp.data->index); }
        );
    }

    void clear() { bodies.clear(); }

  private:
    void resolveFused(SimulatedPhysicsBody& a, SimulatedPhysicsBody& b) {
        a.pos.x -= a.radius / 2;
        b.pos.x += b.radius / 2;
    }

    void applyCollisionForces(SimulatedPhysicsBody& a, SimulatedPhysicsBody& b) {
        glm::vec2 posA = a.pos + a.vel * stepSize;
        glm::vec2 posB = b.pos + b.vel * stepSize;

        float collisionDist = a.radius + b.radius;

        float currentDist = glm::distance(posA, posB);
        // When nan, currentDist is 0! They fused together. Skip collision check!
        if (std::isnan(currentDist)) {
            resolveFused(a, b);
            return;
        }

        if (currentDist > (collisionDist + 0.1f)) return;

        glm::vec2 normal = glm::normalize(posA - posB);
        // When a & b distance is 0, they are at the same spot, hence no collision normal, so
        // just use up vector
        if (std::isnan(currentDist)) normal = VEC_UP;
        /// Resist penetration
        float pen_depth   = collisionDist - currentDist;
        glm::vec2 pen_res = normal * (pen_depth * .5f + 0.1f);
        a.pos += pen_res;
        b.pos += -pen_res;

        // Collision resolution
        glm::vec2 incoming = a.vel - b.vel;
        float resForce     = -glm::dot(incoming, normal);
        float totalMass    = b.mass + a.mass;
        a.vel += normal * (resForce * b.mass / totalMass);
        b.vel += -normal * (resForce * a.mass / totalMass);
    }

    void applyGravityForce(SimulatedPhysicsBody& a, SimulatedPhysicsBody& b) {
        glm::vec2 dir   = b.pos - a.pos;
        float distance2 = glm::length2(dir);
        if (std::isnan(distance2)) return;
        float sharedForce  = 100.f * gravityConstant * ((a.mass * b.mass) / distance2);
        glm::vec2 dir_norm = glm::normalize(dir);

        a.accelerate(dir_norm, sharedForce * .5f * stepSize);
    }

    void updateBody(SimulatedPhysicsBody& bodyp, int index) {
        for (int j = 0; j < bodies.size(); ++j) {
            if (index == j) continue;  // Skip self
            auto& otherp = bodies[j];

            if (enableCollision) applyCollisionForces(bodyp, otherp);
            if (enableGravity) applyGravityForce(bodyp, otherp);
        }

        bodyp.pos += bodyp.vel * stepSize;
    }
};