// Author: Zackery Mason-Blaug
// Date: 2022-06-10
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

struct material_t {
    std::string shader;
    std::string albedo;
    std::string normal;

    v3f color;
    //bool triplanar; //??

};

