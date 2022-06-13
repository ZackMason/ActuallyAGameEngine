// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <functional>
#include <string>
#include "types.hpp"

#include "Graphics/texture.hpp"

struct texture2d_t : public texture_i {
    int width{0}, height{0};
    GLenum data_format;
    GLenum internal_format;
    GLint channels;
    GLenum data_type = GL_UNSIGNED_BYTE;
    
    void bind() override;
    void bind_image();
    void unbind() override;

    void destroy();
    void set_filter(bool linear);
    void set_wrap(bool wrap);
    void mipmap();

    ~texture2d_t(){ 
        if (id) destroy();
    }

    void set(const std::function<u32(int, int)>& fn);
    void set_data(const std::vector<u32>& data);
    explicit texture2d_t(const std::string& path);
    explicit texture2d_t(u32 w, u32 h);
};

