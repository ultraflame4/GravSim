//
// Created by powew on 20/10/2023.
//

#pragma once
#include <memory>
#include <spdlog/logger.h>
#include "logging.hh"

class VertexObject {
public:
    VertexObject(int byteSize, const float *data, int stride);

    void SetTriangles(int byteSize, const unsigned int *indices);
    void CreateAttrib(int size);

    void bind();
    void draw();
    void drawPoints();

private:
//    static inline std::shared_ptr<spdlog::logger> logger = logging::get<VertexObject>();
    unsigned int vbo;
    unsigned int vao;
    unsigned int ebo;

    int attr_index = 0;
    int attr_offset = 0;
    int attr_stride = 0;
    int vertex_count = 0;
    int triangles_count = 0;
};
