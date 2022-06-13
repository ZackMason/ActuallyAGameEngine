// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <utility>

#include "Util/vector.hpp"
#include "Graphics/bindable.hpp"

template <typename T>
struct buffer_t : bindable_i {
    utl::vector<T> data;

    auto size() const {
        return data.size();
    }

    void bind() override {
        glBindBuffer(GL_ARRAY_BUFFER, id);
    }

    void unbind() override {
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void create() {
        glGenBuffers(1, &id);
        glBindBuffer(GL_ARRAY_BUFFER, id);
        
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(T), data.data(), GL_STATIC_DRAW);
        
    }

    explicit buffer_t(utl::vector<T>&& p_data)
        : data(std::move(p_data))
    {
        create();
    }

    explicit buffer_t(const utl::vector<T>& p_data)
        : data(p_data)
    {
        create();
    }

    buffer_t(buffer_t&) = delete;
    buffer_t(buffer_t&&) = delete;
    buffer_t& operator=(buffer_t&) = delete;
    buffer_t& operator=(buffer_t&&) = delete;
};

