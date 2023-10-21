//
// Created by powew on 21/10/2023.
//

#pragma once
#include <glm/glm.hpp>
#include "VertexObject.hh"

class Line {
public:
    glm::vec2 origin = glm::vec2(0,0);
    glm::vec3 direction = glm::vec3(0,0,0);

    float thick = 5;
    bool active = true;

    void load();
    void update();
    void draw(glm::mat4 view, glm::mat4 proj);
    ~Line();

private:
    float vertices[8];
    unsigned int triangles[6];
    VertexObject* vo = nullptr;

};
