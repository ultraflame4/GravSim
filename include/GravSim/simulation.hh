#pragma once

#include <glm/glm.hpp>
#include <vector>

struct SimulatedPhysicsBody {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;

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
    float stepSize = 1;

  public:
    Simulation() {}

    int spawnBody(float x, float y, float radius, float mass) {
        auto& bodyp = this->bodies.emplace_back(
            SimulatedPhysicsBody{glm::vec2(x, y), glm::vec2(x, y), mass, radius}
        );

        return bodies.size() - 1;
    }
};