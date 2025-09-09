//
// Created by powew on 20/10/2023.
//

#pragma once

#include <memory>
#include "vertex_object.hh"

class Square{
public:
    static inline float vertices[] = {
            -0.5, 0.5,
            0.5, 0.5,
            0.5, -0.5,
            -0.5, -0.5

    };
    static inline unsigned int indices[] = {
            2,1,0,
            0, 3, 2
    };

    static std::unique_ptr<VertexObject> CreateSquare(){
        auto square = std::make_unique<VertexObject>(sizeof(vertices), vertices, 2 * sizeof(float));
        square->SetTriangles(sizeof(indices), indices);
        square->CreateAttrib(2);
        return square;
    }
};