//
// Created by powew on 21/10/2023.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/norm.hpp>
#include <cmath>
#include <algorithm>
#include <execution>

#include "GravSim/GravitySimulation.hh"
#include "GravSim/utils.hh"

GravitySimulation::GravitySimulation(Window &window) : window(window) {}

void GravitySimulation::load() {
    auto *bodiesArr = reinterpret_cast<float *>(bodies.data());
    int stride = sizeof(GravBodyVertex);
    int size = stride * bodies.size();
    vo = new VertexObject(size, bodiesArr, stride);

    vo->CreateAttrib(2); // Position attribute
    vo->CreateAttrib(1); // radius attribute
    vo->CreateAttrib(3); // color attribute

    shader.addShader("./assets/gravbody.vertex.glsl", ShaderType::VERTEX);
    shader.addShader("./assets/gravbody.fragment.glsl", ShaderType::FRAGMENT);
    shader.build();
    shader.use();

    feathering_loc = shader.getUniformLoc("feathering");
    viewport_loc = shader.getUniformLoc("viewport");
    model_loc = shader.getUniformLoc("model");
    view_loc = shader.getUniformLoc("view");
    proj_loc = shader.getUniformLoc("proj");

}


void GravitySimulation::ApplyGravityForce(GravBodyPhysical &bodyp, GravBodyPhysical &otherp) {
    glm::vec2 dir = otherp.pos - bodyp.pos;
    float distance2 = glm::length2(dir);
    if (std::isnan(distance2)) return;
    float sharedForce = 100.f * gravityConstant * ((bodyp.mass * otherp.mass) / distance2);
    glm::vec2 dir_norm = glm::normalize(dir);
    if (!otherp.phantom) bodyp.Accelerate(dir_norm, sharedForce * .5f * window.updateTimer.delta);
}

void GravitySimulation::ApplyCollisionForces(GravBodyPhysical &bodyp, GravBodyPhysical &otherp) {

    glm::vec2 posA = bodyp.pos + bodyp.vel * window.updateTimer.delta;
    glm::vec2 posB = otherp.pos + otherp.vel * window.updateTimer.delta;

    float collisionDist = bodyp.radius + otherp.radius;

    float currentDist = glm::distance(posA, posB);
    // When nan, currentDist is 0! They fused together. Skip collision check!
    if (std::isnan(currentDist)) {
        resolveFused(bodyp, otherp);
        return;
    }

    if (currentDist > (collisionDist + 0.1f)) return;

    glm::vec2 normal = glm::normalize(posA - posB);
    // When a & b distance is 0, they are at the same spot, hence no collision normal, so just use up vector
    if (std::isnan(currentDist)) normal = up;
    /// Resist penetration
    float pen_depth = collisionDist - currentDist;
    glm::vec2 pen_res = normal * (pen_depth * .5f + 0.1f);
    if (!otherp.phantom) bodyp.pos += pen_res;
    if (!bodyp.phantom) otherp.pos += -pen_res;

    // Collision resolution
    glm::vec2 incoming = bodyp.vel - otherp.vel;
    float resForce = -glm::dot(incoming, normal);
    float totalMass = otherp.mass + bodyp.mass;
    if (!otherp.phantom) bodyp.vel += normal * (resForce * otherp.mass / totalMass);
    if (!bodyp.phantom) otherp.vel += -normal * (resForce * bodyp.mass / totalMass);
}

void GravitySimulation::UpdateGravBodyPhysics(GravBodyPhysical &bodyp, int index) {
    if (!bodyp.active) return;

    for (int j = 0; j < bodies.size(); ++j) {
        if (index == j) continue; // Skip self
        auto &otherp = physicalBodies[j];
        if (!otherp.active) continue;
        if (gravity)ApplyGravityForce(bodyp, otherp);
    }

    if (collision){
        auto quad = quadTreeManager.Raycast(bodyp.pos, bodyp.vel * window.updateTimer.delta);

        if (!quad.IsEmpty()) {
            quad.square.colliding= true;
//        logger->debug("Node d{} s{} {},{}",quad.node->depth,quad.square.size,quad.square.center.x,quad.square.center.y);
            for (auto otherp_index : quad.node->items) {
                if (bodyp.index == otherp_index){
                    continue;
                }
                ApplyCollisionForces(bodyp, physicalBodies[otherp_index]);
            }
        }

    }

    bodyp.pos += bodyp.vel * window.updateTimer.delta;
    bodyp.last_pos = bodyp.pos;

}

void GravitySimulation::update() {

    const float cellSize = 300;

    bodies_mutex.lock();
    quadTreeGenTimer.tick(true);
    glm::vec2 totalPos(0, 0);
    glm::vec2 start(0, 0);
    glm::vec2 end(0, 0);
    for (const auto &bodyp: physicalBodies) {
        totalPos += bodyp.pos;
        start.x = std::min(start.x,bodyp.pos.x);
        start.y = std::min(start.y,bodyp.pos.y);
        end.x = std::max(end.x,bodyp.pos.x);
        end.y = std::max(end.y,bodyp.pos.y);
    }
    glm::vec2 avgPos = totalPos / (float)physicalBodies.size();

    quadTreeManager.center.x = avgPos.x;
    quadTreeManager.center.y = avgPos.y;
    quadTreeManager.physicalSize = std::max(abs(end.x - start.x), abs(end.y - start.y));
    int depth = round(std::log2(quadTreeManager.physicalSize / cellSize));
    quadTreeManager.clearItems();

    for (const auto &bodyp: physicalBodies) {
        auto quad = quadTreeManager.GetOrCreateQuad(bodyp.pos, depth);
//        logger->debug("Body index: {}", bodyp.index);
        quad.node->items.push_back(bodyp.index);
    }


    quadTreeGenTimer.tick();

    physicsTimer.tick(true);
    std::for_each(std::execution::par, physicalBodies.begin(), physicalBodies.end(),
                  [this](GravBodyPhysical &bodyp) {
                      auto &body = this->vertex(bodyp);
                      UpdateGravBodyPhysics(bodyp, bodyp.index);
                      body.x = bodyp.pos.x;
                      body.y = bodyp.pos.y;
                  });
    physicsTimer.tick();

//    for (int i = 0; i < bodies.size(); ++i) {
//        auto &body = bodies[i];
//        auto &bodyp = physicalBodies[i];
//        UpdateGravBodyPhysics(bodyp, i);
//        body.x = bodyp.pos.x;
//        body.y = bodyp.pos.y;
//    }
    bodies_mutex.unlock();
}


void GravitySimulation::draw(glm::mat4 view, glm::mat4 proj) {
    auto *bodiesArr = reinterpret_cast<float *>(bodies.data());
    int stride = sizeof(GravBodyVertex);
    int size = stride * bodies.size();

    glm::mat4 model = glm::mat4(1.0f);


    vo->SetVertices(bodiesArr, size);


    shader.use();
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));

    glUniform2f(viewport_loc, window.width, window.height);
    glUniform1f(feathering_loc, 1);
    vo->drawPoints();

    if (debug) {
        drawDebugLines(view, proj);
    }
    debugLines.Draw();
}

int GravitySimulation::AddBody(float x, float y, float radius, float mass, float color[3], bool active) {
    bodies_mutex.lock();
    bodies.push_back(GravBodyVertex{
            x, y,
            radius,
            color[0], color[1], color[2]
    });
    auto &bodyp = physicalBodies.emplace_back(GravBodyPhysical{
            glm::vec2(x, y),
            glm::vec2(x, y),
            glm::vec2(0, 0),
            mass,
            radius,
            active,
            false
    });
    bodyp.index = physicalBodies.size() - 1;
    bodies_mutex.unlock();
    return bodyp.index;
}

void GravitySimulation::drawDebugLines(glm::mat4 view, glm::mat4 proj) {

    debugLines.proj = &proj;
    debugLines.view = &view;

    for (int i = 0; i < physicalBodies.size(); ++i) {
        auto &bodyp = physicalBodies[i];
        debugLines.CreateRay(bodyp.pos, bodyp.vel, {.4f, .9f, .1f});
    }


    QuadTree::Quad<int> current = quadTreeManager.GetRootQuad();
    QuadTree::Quad<int> childQuad = current;

    int depth = 0;
    int index = 0;
    while (true) {


        if (index >= 4) {
            // Set next index to the parent's sibling
            index = current.node->index + 1;
            // Ascend
            current = current.getparent();

            depth--;
            // logger->debug("Ascend to depth {} index {}", depth, index);
            continue;
        }
        if (current.IsEmpty()) break;

        childQuad = current.getchild(index);
        // If current is null, increase index
        if (childQuad.IsEmpty()) {
            index++;
            // logger->debug("Current Is null. Change to depth {} index {}", depth, index);
            continue;
        }
        float r = sin(depth+1);
        float g = cos(depth);
        r = r / 2 + .5f;
        g = g / 2 + .5f;
        if (childQuad.square.colliding) {
            debugLines.CreateSquare(childQuad.square.center, childQuad.square.size, {1, 1, 1}, 30);
        }
        else{
            debugLines.CreateSquare(childQuad.square.center, childQuad.square.size, {r, g, .5}, 3);
        }

        // Set child as new current
        current = childQuad;
        index = 0;
        depth++;

        // logger->debug("Descend to depth {} index {}", depth, index);
    }
}

void GravitySimulation::clear() {
    bodies_mutex.lock();
    physicalBodies.clear();
    bodies.clear();
    bodies_mutex.unlock();
}

GravBodyVertex &GravitySimulation::vertex(GravBodyPhysical &bodyp) {
    return bodies[bodyp.index];
}

void GravitySimulation::resolveFused(GravBodyPhysical &a, GravBodyPhysical &b) {
    if (!b.phantom) a.pos.x -= a.radius / 2;
    if (!a.phantom) b.pos.x += b.radius / 2;
}

