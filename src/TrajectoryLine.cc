//
// Created by powew on 21/10/2023.
//

#include "GravSim/TrajectoryLine.hh"
#include "GravSim/window.hh"
#include "GravSim/square.hh"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>


void Line::load() {
    shader.addShader("./assets/line.vertex.glsl", ShaderType::VERTEX);
    shader.addShader("./assets/line.fragment.glsl", ShaderType::FRAGMENT);
    shader.build();
    shader.use();
    model_loc = shader.getUniformLoc("model");
    view_loc = shader.getUniformLoc("view");
    proj_loc = shader.getUniformLoc("proj");
    color_loc = shader.getUniformLoc("color");

}

void Line::update() {

    float length = glm::length(direction);
    float half = thick/2;
    vertices[0] = 1.f * half;
    vertices[1] = -1.f;
    vertices[2] = -1.f * half;
    vertices[3] = -1.f;
    vertices[4] = -1.f * half;
    vertices[5] = fmax(length , 1.0);
    vertices[6] = 1.f * half;
    vertices[7] = fmax(length , 1.0);

    triangles[0] = 2;
    triangles[1] = 1;
    triangles[2] = 3;
    triangles[3] = 3;
    triangles[4] = 1;
    triangles[5] = 0;

    if (vo == nullptr) {
        vo = new VertexObject(sizeof(vertices), vertices, sizeof(float) * 2);
        vo->CreateAttrib(2);// Position
    }


    vo->SetVertices(vertices, sizeof(vertices));
    vo->SetTriangles(sizeof(triangles), triangles);
}

const auto up = glm::vec3(0, 1, 0);
//const auto right = glm::vec3(1, 0, 0);
//const auto forward = glm::vec3(0, 0, 1);

void Line::draw(glm::mat4 view, glm::mat4 proj) {
    if (!active) return;


    glm::qua qat = glm::rotation(up,glm::normalize(direction));
//    glm::qua qat(glm::vec3(0.0, 0.0, 0.0));
    glm::mat4 model = glm::mat4(1.f);
    model = glm::translate(model, origin);
    model = model * glm::mat4_cast(qat);

    shader.use();
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(color_loc, color[0],color[1],color[2]);
    vo->draw();
}

Line::~Line() {
    delete vo;
}







