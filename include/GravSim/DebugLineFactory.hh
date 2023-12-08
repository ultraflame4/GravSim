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


struct DebugLine{
    glm::vec2 origin;
    glm::vec2 dest;
    Color color;
    float thickness;
};

struct DebugLineVertex{
    glm::vec2 aPos;
    Color color;
};
struct DebugLineTriangle{
    int a;
    int b;
    int c;
};

class DebugLineFactory{
    std::vector<DebugLine> lines;
    VertexObject* vo = nullptr;
public:
    glm::mat4* view;
    glm::mat4* proj;
    static inline Shader shader;
    static void load(){
        shader.addShader("./assets/debugline.vertex.glsl", ShaderType::VERTEX);
        shader.addShader("./assets/debugline.fragment.glsl", ShaderType::FRAGMENT);
    }


    void CreateLine(glm::vec2 origin, glm::vec2 dest, Color color = {.5f, .5f, .5f}, float thick = 1){
        lines.push_back(DebugLine(origin, dest, color, thick));
    }

    void Draw(){
        int vertexCount = lines.size() * 4;
        int triCount = lines.size() * 2;
        DebugLineVertex *lineVertices = new DebugLineVertex[vertexCount];
        DebugLineTriangle *triangles = new DebugLineTriangle[triCount];

        for (int i = 0; i < lines.size(); ++i) {
            DebugLine line = lines[i];
            glm::vec2 dir = glm::normalize(line.dest - line.origin);
            glm::vec2 right = glm::vec2(dir.y,-dir.x) * line.thickness * .5f;
            // Create the vertices for the lines
            lineVertices[i].aPos = line.dest - right;
            lineVertices[i+1].aPos = line.dest + right;
            lineVertices[i+2].aPos = line.origin - right;
            lineVertices[i+3].aPos = line.origin + right;

            triangles[i] = {i,i+2,i+3};
            triangles[i+1] = {i+3,i+1,i};
        }

        if (vo == nullptr) {
            vo = new VertexObject(sizeof(DebugLineVertex) * vertexCount, reinterpret_cast<const float *>(lineVertices), sizeof(DebugLineVertex));
            vo->CreateAttrib(2); // Position
            vo->CreateAttrib(3); // Color
        }
        vo->SetVertices(reinterpret_cast<const float *>(lineVertices), sizeof(DebugLineVertex) * vertexCount);
        vo->SetTriangles(sizeof(DebugLineTriangle) * triCount, reinterpret_cast<const unsigned int *>(triangles));
        vo->draw();
        lines.clear();


    }

    void CreateRay(glm::vec3 pos, glm::vec3 direction, Color color = {.5f, .5f, .5f}, float thick = 1){
        CreateLine(pos, pos + direction, color, thick);
    }

    void CreateRay(glm::vec2 pos, glm::vec2 direction, Color color = {.5f, .5f, .5f}, float thick = 1){
        CreateRay(glm::vec3(pos.x, pos.y, 0), glm::vec3(direction.x, direction.y, 0), color, thick);
    }

    void CreateSquare(glm::vec2 center, float width, Color color = {.5f, .5f, .5f}, float thick = 1) {
        glm::vec2 up = glm::vec2(0, width);
        glm::vec2 right = glm::vec2( width,0);

        glm::vec2 tl = center+(-right+up)*.5f;
        glm::vec2 br = center+(right-up)*.5f;


        CreateRay(tl, right, color, thick);
        CreateRay(tl - up, up, color, thick);
        CreateRay(tl - up, right, color, thick);
        CreateRay(br, up, color, thick);

    }
};