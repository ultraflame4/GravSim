//
// Created by powew on 20/10/2023.
//

#pragma once
#include <memory>
#include <spdlog/logger.h>
#include "logging.hh"

class VertexObject {
  public:
    /**
     * @brief Construct a new Vertex Object
     *
     * @param byteSize Size of a single vertex * number of items in data
     * @param data Starting data
     * @param stride Size of each vertex.
     */
    VertexObject(int byteSize, const float* data, int stride);

    void SetVertices(const float* dataArr, int byteSize);
    void SetTriangles(int byteSize, const unsigned int* indices);
    void CreateAttrib(int size);

    void bind();
    void draw();
    void drawPoints();

    ~VertexObject();

  private:
    //    static inline std::shared_ptr<spdlog::logger> logger = logging::get<VertexObject>();
    unsigned int vbo;
    unsigned int vao;
    std::optional<unsigned int> ebo = std::nullopt;

    int attr_index      = 0;
    int attr_offset     = 0;
    int attr_stride     = 0;
    int vertex_count    = 0;
    int triangles_count = 0;
};
