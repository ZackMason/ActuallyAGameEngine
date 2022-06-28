// Author: Zackery Mason-Blaug
// Date: 2022-06-27
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include "Util/vector.hpp"

struct bloom_framebuffer_t {
    struct bloom_mip_t {
        v2f fsize;
        v2i size;
        unsigned int texture{0};
    };

    void bind();
    void resize(int, int);

    bloom_framebuffer_t(u32 w, u32 h, u32 mip_length);
    ~bloom_framebuffer_t();

    
    unsigned int width,height;
    unsigned int id{0};
    utl::vector<bloom_mip_t> mip_chain;
};

