// Author: Zackery Mason-Blaug
// Date: 2022-06-10
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/buffer.hpp"

#include "glad/glad.h"

template <typename T>
using shared_buffer_t = buffer_t<T>;

template <typename T>
shared_buffer_t<T> create_shared_buffer(const T& data) {
    return shared_buffer_t<T>(data, GL_UNIFORM_BUFFER);
}

template <typename T>
void set_shared_buffer(buffer_t<T>& buffer, const T* data) {
    //buffer.bind();
    //glBufferSubData(buffer.type, 0, data->size() * sizeof(T::value_type), data);
    glNamedBufferSubData(buffer.id, 0, data->size() * sizeof(typename T::value_type), data);
}



template <typename T>
void bind_buffer(const shared_buffer_t<T>& buffer, int slot) {
    //glBindBufferBase(GL_UNIFORM_BUFFER, slot, buffer.id);
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, buffer.id);
}