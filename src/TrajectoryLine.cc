//
// Created by powew on 21/10/2023.
//

#include "GravSim/TrajectoryLine.hh"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/quaternion.hpp>


void Line::load() {


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

    vo->draw();
}

Line::~Line() {
    delete vo;
}







