//
// Created by powew on 20/10/2023.
//
#include <glad/glad.h>
#include <iostream>
#include "GravSim/VertexObject.hh"
#include "GravSim/window.hh"

VertexObject::VertexObject(int byteSize, const float *data, int stride) {
    glGenBuffers(1, &vbo);
    attr_stride = stride;
    SetVertices(data, byteSize);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    CheckGLErrors();
}
void VertexObject::SetVertices(const float *dataArr, int byteSize) {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, dataArr, GL_DYNAMIC_DRAW);
    vertex_count= byteSize/attr_stride;
}

void VertexObject::CreateAttrib(int size) {
//    logger->trace("Create attrib at index {}, size {}, stride {}, offset {}", attr_index,size,attr_stride,attr_offset);
    glVertexAttribPointer(attr_index, size, GL_FLOAT, GL_FALSE, attr_stride, (void*)(attr_offset * sizeof(float )));

    glEnableVertexAttribArray(attr_index);
    attr_offset += size;
    attr_index++;
}

void VertexObject::bind() {
    glBindVertexArray(vao);
}

void VertexObject::draw() {
    bind();
    if (!ebo.has_value()) {
        throw std::runtime_error("Triangles not set!");
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.value());
    glDrawElements(GL_TRIANGLES, triangles_count, GL_UNSIGNED_INT, nullptr);
}

void VertexObject::drawPoints() {
    bind();
    glDrawArrays( GL_POINTS, 0, vertex_count );
}


void VertexObject::SetTriangles(int byteSize, const unsigned int *indices) {
    if (!ebo.has_value()) {
        ebo = 0;
        glGenBuffers(1, &*ebo);
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo.value());
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteSize, indices, GL_DYNAMIC_DRAW);
    triangles_count = byteSize / sizeof (unsigned int);
}

VertexObject::~VertexObject() {
    glDeleteBuffers(1,&vao);
    glDeleteBuffers(1,&vbo);
    if (ebo.has_value()) {
        glDeleteBuffers(1,&*ebo);
    }
}


