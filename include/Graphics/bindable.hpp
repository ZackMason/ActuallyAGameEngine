// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <glad/glad.h>

struct gl_handle_t {
    GLuint id;
};

struct bindable_i : public gl_handle_t {
    virtual void bind() = 0;
    virtual void unbind() = 0;

    virtual ~bindable_i() = default;
};

struct drawable_i : bindable_i {
    virtual void draw() = 0;
    virtual ~drawable_i() = default;
};
