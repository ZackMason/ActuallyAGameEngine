// Author: Zackery Mason-Blaug
// Date: 2022-06-10
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Util/string.hpp"

struct material_t {
    v3f base_color{1.0f};
    float metallic{0.0f};
    float roughness{0.5f};
    float emissive{0.0f};

    sid_t albedo_texture{0};
    //bool triplanar; //??

};

