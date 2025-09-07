//
// Created by powew on 21/10/2023.
//

#pragma once
#include <glm/glm.hpp>
#include "vertex_object.hh"
#include "shader.hh"

class Line {
public:
    glm::vec3 origin = glm::vec3(0,0,0);
    glm::vec3 direction = glm::vec3(0,0,0);

    float thick = 5;
    float color[3] = {.5f,.5f,.5f};
    bool active = true;

    static void load();
    void update_line_vertices();
    void draw(glm::mat4 view, glm::mat4 proj);
    ~Line();

private:
    float vertices[8];
    unsigned int triangles[6];
    VertexObject* vo = nullptr;
    static inline Shader shader;
    static inline int color_loc;
    static inline int proj_loc;
    static inline int view_loc;
    static inline int model_loc;
};
