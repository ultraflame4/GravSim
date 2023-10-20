//
// Created by powew on 20/10/2023.
//

#pragma once
#include <glm/glm.hpp>

struct GravBodyVertex{
    float x;
    float y;
    float radius;
    float r;
    float g;
    float b;
};
static_assert(sizeof(GravBodyVertex) == sizeof (float ) * 6, "ERROR GravBodyVertex struct contains padding!");

struct GravBodyPhysical{
    glm::vec2 pos;
    glm::vec2 vel;
    float mass;
    float radius;
};