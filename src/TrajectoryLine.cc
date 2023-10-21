//
// Created by powew on 21/10/2023.
//

#include "GravSim/TrajectoryLine.hh"
#include "GravSim/window.hh"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>


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
    vertices[0] = -1.f * thick;
    vertices[1] = -1.f;
    vertices[2] = -1.f * thick;
    vertices[3] = -1.f;
    vertices[4] = -1.f * thick;
    vertices[5] = length;
    vertices[6] = -1.f * thick;
    vertices[7] = length;

    triangles[0] = 2;
    triangles[0] = 1;
    triangles[0] = 3;
    triangles[0] = 3;
    triangles[0] = 1;
    triangles[0] = 0;

    if (vo == nullptr) {
        vo = new VertexObject(sizeof(vertices), vertices, sizeof(float) * 2);
        vo->CreateAttrib(2);// Position
    }


    vo->SetVertices(vertices, sizeof(vertices));
    vo->SetTriangles(sizeof(triangles), triangles);
}

const auto up = glm::vec3(0, 1, 0);

void Line::draw(glm::mat4 view, glm::mat4 proj) {
    if (!active) return;
    update();
    glm::qua qat = glm::quatLookAt(direction, up);
    glm::mat4 model = glm::toMat4(qat);

    shader.use();
    glUniformMatrix4fv(model_loc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(proj_loc, 1, GL_FALSE, glm::value_ptr(proj));
    glUniform3f(color_loc, .5f,.9f,.9f);
    vo->draw();
}

Line::~Line() {
    delete vo;
}







