// Author: Zackery Mason-Blaug
// Date: 2022-06-23
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include <cstdlib>
#include <string_view>

using color4 = v4f;
using color3 = v3f;
using color32 = u32;

// takes in "#fefefefe"
constexpr color32 operator"" _rgba(const char* str, std::size_t size) {
    //static_assert(size == 9, "invalid rgba size");
    char* end;
    return std::strtoul(str+1, &end, 16);
    //auto r = std::strtoul(str + 1, str + 3, 16);
    //auto g = std::strtoul(str + 3, str + 5, 16);
    //auto b = std::strtoul(str + 5, str + 7, 16);
    //auto a = std::strtoul(str + 7, str + 9, 16);
    //return r    << 0 | g << 8 | b << 16 | a << 24;
}

namespace colors {
    static auto clear = "0x00000000"_rgba;
    static auto black = "0x000000ff"_rgba;
    static auto white = "0xffffffff"_rgba;
    static auto red   = "0xff0000ff"_rgba;
    static auto green = "0x00ff00ff"_rgba;
    static auto blue  = "0x0000ffff"_rgba;
    static auto yellow= "0xffff00ff"_rgba;
    static auto sand  = "0xC2B280ff"_rgba;
};



