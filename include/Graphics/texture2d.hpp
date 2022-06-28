// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <functional>
#include <string>
#include "types.hpp"

#include "Graphics/texture.hpp"


// TODO: allow of async access
// std::future??
// call back?? 
// delay timer trick?
struct texture2d_accessor_t {
    GLubyte* data{nullptr};
    GLuint pbo{0};
    int width{0}, height{0};
    GLenum format;

    // texture needs to be bound first
    explicit texture2d_accessor_t(GLuint id, int w, int h, GLenum f, GLuint mip_level=0);
    virtual ~texture2d_accessor_t()// = default;
    {
        glUnmapBuffer(GL_PIXEL_PACK_BUFFER);
        glDeleteBuffers(1, &pbo);
    }
};

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

    texture2d_accessor_t map_buffer();

    ~texture2d_t(){ 
        if (id) destroy();
    }

    void set(const std::function<u32(int, int)>& fn);
    void set_data(const std::vector<u32>& data);
    explicit texture2d_t(): 
        texture2d_t("textures/UVgrid.png", CMAKE_ASSETS_PATH)
    {}
    
    explicit texture2d_t(const std::string& path, const std::string& asset_dir);
    explicit texture2d_t(u32 w, u32 h);
};

