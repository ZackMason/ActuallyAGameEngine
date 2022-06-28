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
void bind_buffer(const shared_buffer_t<T>& buffer, int slot) {
    glBindBufferBase(GL_UNIFORM_BUFFER, slot, buffer.id);
}