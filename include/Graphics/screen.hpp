// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <glad/glad.h>

#include "Graphics/buffer.hpp"
#include "Graphics/vertex_array.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/texture.hpp"

struct screen_t {
    struct vertex_t {
        v2f pos, uv;
    };

    screen_t(){
        buffer.bind();
        vertex_array.bind_ref()
            .set_attrib(0, 2, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, pos))
            .set_attrib(1, 2, GL_FLOAT, sizeof(vertex_t), offsetof(vertex_t, uv))
            .unbind();
    }

    void draw() {
        vertex_array.draw();
    }

    buffer_t<utl::vector<vertex_t>> buffer{{
        {{-1,-1}, {0,0}},
        {{1,-1}, {1,0}},
        {{-1,1}, {0,1}},

        {{1,1}, {1,1}},
        {{-1,1}, {0,1}},
        {{1,-1}, {1,0}},
    }};
    vertex_array_t vertex_array{buffer.size()};
};

