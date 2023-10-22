//
// Created by powew on 21/10/2023.
//

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <algorithm>

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
    float distance = glm::distance(bodyp.pos, otherp.pos);
    if (isnan(distance)) return;
    float sharedForce = gravityConstant * ((bodyp.mass * otherp.mass) / pow(distance, 2));
    glm::vec2 dirVector = glm::normalize(otherp.pos - bodyp.pos);
    bodyp.Accelerate(dirVector, sharedForce / 2);
}

void GravitySimulation::ApplyCollisionForces(GravBodyPhysical &bodyp, GravBodyPhysical &otherp) {

    glm::vec2 posA = bodyp.pos + bodyp.vel;
    glm::vec2 posB = otherp.pos + bodyp.vel;

    float collisionDist = bodyp.radius + otherp.radius;

    float currentDist = glm::distance(posA, posB);
    // When nan, currentDist is 0! They fused together. Skip collision check!
    if (isnan(currentDist)) {
        resolveFused(bodyp, otherp);
        return;
    }

    if (currentDist > (collisionDist + 0.1f)) return;

    glm::vec2 normal = glm::normalize(posA - posB);
    // When a & b distance is 0, they are at the same spot, hence no collision normal, so just use up vector
    if (isnan(currentDist)) normal = up;
    /// Resist penetration
    float pen_depth = collisionDist - currentDist;
    glm::vec2 pen_res = normal * (pen_depth *.5f + 0.1f);
    bodyp.pos += pen_res;
    otherp.pos += -pen_res;

    // Collision resolution
    glm::vec2 incoming = bodyp.vel - otherp.vel;
    float resForce = -glm::dot(incoming, normal);
    float totalMass = otherp.mass + bodyp.mass;
    bodyp.vel += normal * (resForce * otherp.mass/totalMass);
    otherp.vel += -normal * (resForce * bodyp.mass/totalMass);
}

void GravitySimulation::UpdateGravBodyPhysics(GravBodyPhysical &bodyp, int index) {
    if (!bodyp.active) return;

    for (int j = 0; j < bodies.size(); ++j) {
        if (index == j) continue; // Skip self
        auto &otherp = physicalBodies[j];
        if (!otherp.active) continue;
        if (gravity)ApplyGravityForce(bodyp, otherp);
        if (collision) ApplyCollisionForces(bodyp, otherp);
    }

    bodyp.pos += bodyp.vel * window.updateTimer.delta;
    bodyp.last_pos = bodyp.pos;

}

void GravitySimulation::update() {

    // Make circles go in circle. this is temp for testing
    for (int i = 0; i < bodies.size(); ++i) {
        auto &body = bodies[i];
        auto &bodyp = physicalBodies[i];
        UpdateGravBodyPhysics(bodyp, i);
        body.x = bodyp.pos.x;
        body.y = bodyp.pos.y;
    }
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
}

GravBodyPhysical &GravitySimulation::AddBody(float x, float y, float radius, float mass, float color[3], bool active) {
    bodies.push_back(GravBodyVertex{
            x, y,
            radius,
            color[0], color[1], color[2]
    });
    auto &bodyp = physicalBodies.emplace_back(GravBodyPhysical{
            active,
            glm::vec2(x, y),
            glm::vec2(x, y),
            glm::vec2(0, 0),
            mass,
            radius,
    });
    bodyp.index = physicalBodies.size()-1;
    return bodyp;
}

void GravitySimulation::drawDebugLines(glm::mat4 view, glm::mat4 proj) {
    if (debugLines.size() != physicalBodies.size()) {
        debugLines.clear();
        for (const auto &item: physicalBodies) {
            auto &line = debugLines.emplace_back();
            line.color[0] = 0.4f;
            line.color[1] = 0.9f;
            line.color[2] = 0.1f;
            line.thick = 1;
        }
    }

    for (int i = 0; i < physicalBodies.size(); ++i) {
        auto &line = debugLines[i];
        auto &bodyp = physicalBodies[i];
        line.direction.x = bodyp.vel.x;
        line.direction.y = bodyp.vel.y;
        line.origin.x = bodyp.pos.x;
        line.origin.y = bodyp.pos.y;
        line.update();
        line.draw(view, proj);
    }
}

void GravitySimulation::clear() {
    physicalBodies.clear();
    bodies.clear();
    debugLines.clear();
}

GravBodyVertex & GravitySimulation::vertex(GravBodyPhysical &bodyp) {
    return bodies[bodyp.index];
}

void GravitySimulation::resolveFused(GravBodyPhysical &a, GravBodyPhysical &b) {
    a.pos.x -= a.radius/2;
    b.pos.x += b.radius/2;
}

