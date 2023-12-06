//
// Created by powew on 5/12/2023.
//

#pragma once
#include <glm/glm.hpp>
#include "Line.hh"

struct Color{
    float r;
    float g;
    float b;
};

class DebugLineFactory{
    std::vector<Line> lines;
public:
    glm::mat4* view;
    glm::mat4* proj;

    void DrawRay(glm::vec3 pos, glm::vec3 direction, Color color = {.5f,.5f,.5f}, float thick = 1){
        Line line;
        line.active= true;
        line.color[0] = color.r;
        line.color[1] = color.g;
        line.color[2] = color.b;
        line.thick = thick;

        line.direction= direction;
        line.origin = pos;
        line.update_line_vertices();
        line.draw(*view, *proj);
    }

    void DrawRay(glm::vec2 pos, glm::vec2 direction, Color color = {.5f,.5f,.5f}, float thick = 1){
        DrawRay(glm::vec3(pos.x,pos.y,0),glm::vec3(direction.x,direction.y,0),color,thick);
    }

    void DrawSquare(glm::vec2 center,float width, Color color = {.5f,.5f,.5f}, float thick = 1) {
        glm::vec2 up = glm::vec2(0, width);
        glm::vec2 right = glm::vec2( width,0);

        glm::vec2 tl = center+(-right+up)*.5f;
        glm::vec2 br = center+(right-up)*.5f;



        DrawRay(tl, right, color, thick);
        DrawRay(tl-up, up, color, thick);
        DrawRay(tl-up, right, color, thick);
        DrawRay(br, up, color, thick);

    }
};