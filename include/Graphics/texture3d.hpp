// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Graphics/texture.hpp"

struct texture3d_t : public texture_i {
    int width{0}, height{0}, depth{0};

    void bind() override;
    void unbind() override;
    void generate_mipmaps();

    void destroy();

    void set_data(std::function<void(texture3d_t&)> func);
    texture3d_t(const std::string& path);
    texture3d_t(u32 w, u32 h, u32 d);
};

