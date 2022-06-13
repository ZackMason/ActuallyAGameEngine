// Author: Zackery Mason-Blaug
// Date: 2022-06-10
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/bindable.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/texture2d.hpp"

#include <string>

struct texture_compute_t{

    void bind_buffer(unsigned int buffer);
    void execute(unsigned int count);

    shader_t shader;
    //texture2d_t input;
    texture2d_t output;


    unsigned int group_size{10};

    unsigned int buffer_count{0};

    explicit texture_compute_t(const std::string& file, int w, int h);
};

