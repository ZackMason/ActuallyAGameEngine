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
    
    void draw_elements(const size_t& buffer_size) {
        bind();
            glDrawElements(topology, static_cast<GLsizei>(buffer_size), GL_UNSIGNED_INT, 0);
        unbind();
    }

    void unbind();
    void bind();


    static void reset();
    vertex_array_t& bind_ref() {
        //bind();
        return *this;
    }

    vertex_array_t& set_attrib(int index, int count, GLenum type, GLuint offset) {
        glEnableVertexArrayAttrib(id, index);
        glVertexArrayAttribFormat(id, index, count, type, GL_FALSE, (offset));
        glVertexArrayAttribBinding(id, index, 0);
        //glEnableVertexAttribArray(index);
        //glVertexAttribPointer(index, count, type, GL_FALSE, s, reinterpret_cast<void*>(offset));
        return *this;
    }

    vertex_array_t& set_attribi(int index, int count, GLenum type, GLuint offset) {
        glEnableVertexArrayAttrib(id, index);
        glVertexArrayAttribIFormat(id, index, count, type, offset);
        glVertexArrayAttribBinding(id, index, 0);
        //glEnableVertexAttribArray(index);
        //glVertexAttribIPointer(index, count, type, s, offset);
        return *this;
    }

    vertex_array_t& set_attrib_divisor(int index, int div) {
        glVertexArrayBindingDivisor(id, index, div);
    }

    void create() {
        glCreateVertexArrays(1, &id);
    }

    void destroy() {
        if (id != -1) {
            glDeleteVertexArrays(1, &id);
        }
        id = -1;
    }

    void bind_vertex_buffer(GLuint vbo, int index, size_t vertex_size) {
        glVertexArrayVertexBuffer(id, index, vbo, 0, static_cast<GLsizei>(vertex_size));
    }
    void bind_element_buffer(GLuint ibo) {
        glVertexArrayElementBuffer(id, ibo);
    }

    virtual ~vertex_array_t() {
        destroy();
    }

    explicit vertex_array_t(GLuint vbo, size_t s, size_t vertex_size) : size(static_cast<GLsizei>(s)) {
        create();
        bind_vertex_buffer(vbo, 0, vertex_size);
    }

    explicit vertex_array_t(GLuint vbo, GLuint ibo, size_t s, size_t vertex_size) : size(static_cast<GLsizei>(s)) {
        create();
        bind_vertex_buffer(vbo, 0, vertex_size);
        bind_element_buffer(ibo);
    }

    vertex_array_t(vertex_array_t&) = delete;
    vertex_array_t(vertex_array_t&&) = delete;
    vertex_array_t& operator=(vertex_array_t&) = delete;
    vertex_array_t& operator=(vertex_array_t&&) = delete;
};

