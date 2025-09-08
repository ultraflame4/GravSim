#pragma once
#include <algorithm>
#include <execution>
#include "GravSim/utils.hh"
#include <glm/fwd.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/glm.hpp>
#include <memory>
#include <mutex>
#include <variant>
#include <vector>

struct SimulatedPhysicsBody {
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;
    int id;

    void accelerate(glm::vec2 direction, float forceAmt) {
        vel += direction * (forceAmt / mass);
    }

    void addImmediateForce(glm::vec2 direction, float forceAmt) {
        vel = direction * (forceAmt / mass);
    }
};
struct SimulatedBody {
    int index;
    Color color;
    SimulatedPhysicsBody pbody;  // Data since last copy
};

class Simulation {
  public:
    // TODO: Swap this out for spatial hashing
    std::vector<SimulatedBody> bodies;

    float stepSize        = .01f;
    float gravityConstant = 100.f;
    bool enableCollision  = true;
    bool enableGravity    = true;

    float last_step_delta = 0;
    float last_step_time  = 0;

  public:
    Simulation() {}
    void spawnBody(glm::vec2 xy, float radius, float mass, Color color) {
        spawnBody(xy, glm::vec2(0, 0), radius, mass, color);
    }
    void spawnBody(glm::vec2 xy, glm::vec2 vel, float radius, float mass, Color color) {
        std::lock_guard<std::mutex> lock(mutex_bodies);

        int index = bodies.size();
        auto body =
            SimulatedBody{index, color, SimulatedPhysicsBody{xy, vel, mass, radius, idcounter}};
        idcounter++;
        this->bodies.push_back(body);
    }

    void step() {
        copy_in();

        auto compare_buffer = process_buffer;

        processChunk(process_buffer, compare_buffer);
        copy_out();

        auto now        = glfwGetTime();
        last_step_delta = now - last_step_time;
        last_step_time  = now;
    }

    void clear() {
        std::lock_guard<std::mutex> lock(mutex_bodies);

        bodies.clear();
    }

  private:
    int idcounter;

    std::mutex mutex_bodies;

    // Inner buffer used for calculations.
    std::vector<SimulatedPhysicsBody> process_buffer;
    /**
     * @brief `bodies` into process_buffer for processing
     *
     */
    void copy_in() {
        std::lock_guard<std::mutex> lock(mutex_bodies);

        process_buffer.resize(bodies.size());
        for (int i = 0; i < bodies.size(); i++) { process_buffer[i] = bodies[i].pbody; }
    }
    /**
     * @brief Copies calculated result out of process_buffer into `bodies`
     *
     */
    void copy_out() {
        std::lock_guard<std::mutex> lock(mutex_bodies);
        auto body_count      = bodies.size();
        auto processed_count = process_buffer.size();
        if (body_count < processed_count) {
            process_buffer.resize(body_count);
            processed_count = body_count;
        }
        for (int i = 0; i < processed_count; i++) { bodies[i].pbody = process_buffer[i]; }
    }

    /**
     * @brief Process physics for a chunk
     *
     * @param bodies Bodies to process.
     * @param others Bodies to compare against.
     */
    void processChunk(
        std::vector<SimulatedPhysicsBody>& bodies,
        std::vector<SimulatedPhysicsBody>& others
    ) {
        std::for_each(
            std::execution::par_unseq,
            bodies.begin(),
            bodies.end(),
            [this, others](SimulatedPhysicsBody& bodyp) {
                auto index = bodyp.id;

                for (SimulatedPhysicsBody otherp : others) {
                    if (index == otherp.id) continue;  // Skip self

                    if (enableCollision) applyCollisionForces(bodyp, otherp);
                    if (enableGravity) applyGravityForce(bodyp, otherp);
                }

                bodyp.pos += bodyp.vel * stepSize;
            }
        );
    }

    void applyCollisionForces(SimulatedPhysicsBody& a, SimulatedPhysicsBody& b) {
        glm::vec2 s = a.pos - b.pos;  // relative start pos
        glm::vec2 v = a.vel - b.vel;  // relative velocity
        float R     = a.radius + b.radius;

        float a_coef = glm::dot(v, v);
        float b_coef = 2.0f * glm::dot(s, v);
        float c_coef = glm::dot(s, s) - R * R;

        float tCollide = -1.0f;

        // Already overlapping
        if (c_coef <= 0.0f) {
            tCollide = 0.0f;
        } else if (a_coef > 1e-8f) {
            float disc = b_coef * b_coef - 4.0f * a_coef * c_coef;
            if (disc >= 0.0f) {
                float sqrtD = std::sqrt(disc);
                float t1    = (-b_coef - sqrtD) / (2.0f * a_coef);
                float t2    = (-b_coef + sqrtD) / (2.0f * a_coef);

                // pick earliest valid root within this step
                if (t1 >= 0.0f && t1 <= stepSize)
                    tCollide = t1;
                else if (t2 >= 0.0f && t2 <= stepSize)
                    tCollide = t2;
            }
        }

        // If no collision in this step, bail
        if (tCollide < 0.0f) return;

        // Move both balls to collision point
        glm::vec2 posA = a.pos + a.vel * tCollide;
        glm::vec2 posB = b.pos + b.vel * tCollide;

        float currentDist = glm::distance(posA, posB);
        if (std::isnan(currentDist)) return;

        glm::vec2 normal = glm::normalize(posA - posB);
        if (std::isnan(normal.x) || std::isnan(normal.y)) normal = VEC_UP;

        // Resist penetration
        float pen_depth   = R - currentDist;
        glm::vec2 pen_res = normal * (pen_depth * 0.5f + 0.1f);
        a.pos             = posA + pen_res;
        b.pos             = posB - pen_res;

        // Collision resolution
        glm::vec2 incoming = a.vel - b.vel;
        float resForce     = -glm::dot(incoming, normal);
        float totalMass    = b.mass + a.mass;
        a.vel += normal * (resForce * b.mass / totalMass);
        b.vel -= normal * (resForce * a.mass / totalMass);
    }

    void applyGravityForce(SimulatedPhysicsBody& a, SimulatedPhysicsBody& b) {
        glm::vec2 dir   = b.pos - a.pos;
        float distance2 = glm::length2(dir);
        if (std::isnan(distance2)) return;
        float sharedForce  = 100.f * gravityConstant * ((a.mass * b.mass) / distance2);
        glm::vec2 dir_norm = glm::normalize(dir);

        a.accelerate(dir_norm, sharedForce * .5f * stepSize);
    }
};