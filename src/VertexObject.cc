//
// Created by powew on 20/10/2023.
//
#include <glad/glad.h>
#include "GravSim/VertexObject.hh"

VertexObject::VertexObject(int byteSize, const void *data, int stride) {
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);
    attr_stride = stride;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glBufferData(GL_ARRAY_BUFFER, byteSize, data, GL_STATIC_DRAW);

    triangles_count = byteSize / stride;
}

void VertexObject::CreateAttrib(int size) {
    logger->trace("Create attrib at index {}, size {}, stride {}, offset {}", attr_index,size,attr_stride,attr_offset);
    glVertexAttribPointer(attr_index, size, GL_FLOAT, GL_FALSE, attr_stride, (void*)(attr_offset));

    glEnableVertexAttribArray(attr_index);
    attr_offset += size;
    attr_index++;
}

void VertexObject::bind() {
    glBindVertexArray(vao);
}

void VertexObject::draw() {
    bind();
    glDrawArrays(GL_TRIANGLES, 0, triangles_count);
}
