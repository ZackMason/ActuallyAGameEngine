// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <utility>

#include "Util/vector.hpp"
#include "Graphics/bindable.hpp"

template <typename T>
struct buffer_t : bindable_i {
    GLenum type = GL_ARRAY_BUFFER;
    T data;

    auto size() const {
        return data.size();
    }

    void bind() override {
        glBindBuffer(type, id);
    }

    void unbind() override {
        glBindBuffer(type, 0);
    }

    void create() {
        glGenBuffers(1, &id);
        glBindBuffer(type, id);
        
        glBufferData(type, data.size() * sizeof(T::value_type), data.data(), GL_STATIC_DRAW);
    }

    // only call if you are going to create a new buffer
    void destroy() {
        glDeleteBuffers(1, &id);
    }

    explicit buffer_t(T&& p_data, GLenum p_type)
        : type(p_type), data(std::move(p_data))
    {
        create();
    }

    explicit buffer_t(const T& p_data, GLenum p_type)
        : type(p_type), data(p_data)
    {
        create();
    }

    explicit buffer_t(T&& p_data)
        : data(std::move(p_data))
    {
        create();
    }

    explicit buffer_t(const T& p_data)
        : data(p_data)
    {
        create();
    }

    virtual ~buffer_t() {
        destroy();
    }

    buffer_t() = default;
    buffer_t(buffer_t&) = delete;
    buffer_t(buffer_t&&) = delete;
    buffer_t& operator=(buffer_t&) = delete;
    buffer_t& operator=(buffer_t&&) = delete;
};

