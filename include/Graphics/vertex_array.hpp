// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/buffer.hpp"

struct vertex_array_t : drawable_i {
    GLsizei size;
    GLenum topology = GL_TRIANGLES;

    void draw() {
        bind();
            glDrawArrays(topology, 0, (size));
        unbind();
    }
    
    template <typename T>
        //requires (std::is_same<T::value_type, unsigned int>::value)
    void draw(const buffer_t<T>& buffer) {
        bind();
            glDrawElements(topology, static_cast<GLsizei>(buffer.data.size()), GL_UNSIGNED_INT, 0);
        unbind();
    }

    void unbind();
    void bind();


    static void reset();
    vertex_array_t& bind_ref() {
        bind();
        return *this;
    }

    vertex_array_t& set_attrib(int index, int count, GLenum type, GLsizei s, size_t offset) {
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, count, type, GL_FALSE, s, reinterpret_cast<void*>(offset));
        return *this;
    }

    vertex_array_t& set_attribi(int index, int count, GLenum type, GLsizei s, size_t offset) {
        glEnableVertexAttribArray(index);
        glVertexAttribIPointer(index, count, type, s, reinterpret_cast<void*>(offset));
        return *this;
    }

    void create() {
        glGenVertexArrays(1, &id);
    }

    virtual ~vertex_array_t() {
        glDeleteVertexArrays(1, &id);
    }

    explicit vertex_array_t(size_t s) : size(static_cast<GLsizei>(s)) {
        create();
    }

    explicit vertex_array_t() : size(0) {
        create();
    }
    vertex_array_t(vertex_array_t&) = delete;
    vertex_array_t(vertex_array_t&&) = delete;
    vertex_array_t& operator=(vertex_array_t&) = delete;
    vertex_array_t& operator=(vertex_array_t&&) = delete;
};

