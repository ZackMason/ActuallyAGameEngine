// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/buffer.hpp"

struct vertex_array_t : drawable_i {
    size_t size;
    GLenum topology = GL_TRIANGLES;

    void draw() override {
        bind();
        glDrawArrays(topology, 0, static_cast<GLsizei>(size));
        unbind();
    }
    void unbind();
    void bind();


    static void reset();
    vertex_array_t& bind_ref() {
        bind();
        return *this;
    }

    vertex_array_t& set_attrib(int index, int count, GLenum type, int size, size_t offset) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, count, type, GL_FALSE, size, (void*)offset);
        return *this;
    }

    virtual ~vertex_array_t() {
        glDeleteVertexArrays(1, &id);
    }
    vertex_array_t(size_t s) : size(s) {
        glGenVertexArrays(1, &id);
    }

    vertex_array_t(vertex_array_t&) = delete;
    vertex_array_t(vertex_array_t&&) = delete;
    vertex_array_t& operator=(vertex_array_t&) = delete;
    vertex_array_t& operator=(vertex_array_t&&) = delete;
};

