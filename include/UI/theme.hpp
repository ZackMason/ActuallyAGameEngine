// Author: Zackery Mason-Blaug
// Date: 2022-10-24
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include "Util/color.hpp"

struct theme_t {
    color4 fg_color;
    color4 bg_color;
    color4 text_color;
    color4 shadow_color;

    bool has_shadow{false};
};

