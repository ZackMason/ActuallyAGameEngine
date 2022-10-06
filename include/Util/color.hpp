// Author: Zackery Mason-Blaug
// Date: 2022-06-23
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include <cstdlib>
#include <string_view>
#include <array>

using color4 = v4f;
using color3 = v3f;
using color32 = u32;

namespace color {
    constexpr color32 to_color32(const color4& c) {
        return 
            (u8(c.x * 255.0f) << 0 ) |
            (u8(c.y * 255.0f) << 8 ) |
            (u8(c.z * 255.0f) << 16) |
            (u8(c.w * 255.0f) << 24) ;
    }

    constexpr color32 to_color32(const color3& c) {
        return to_color32(color4(c, 1.0));
    }

    constexpr bool is_hex_digit(char c) {
        return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
    }

    constexpr u32 hex_value(char c) {
        constexpr std::array<std::pair<char, u32>, 22> lut {{
            {'0', 0u}, {'1', 1u}, {'2', 2u}, {'3', 3u}, {'4', 4u},
            {'5', 5u}, {'6', 6u}, {'7', 7u}, {'8', 8u}, {'9', 9u},
            {'a', 10u},{'b', 11u},{'c', 12u},{'d', 13u},{'e', 14u},
            {'f', 15u},{'A', 10u},{'B', 11u},{'C', 12u},{'D', 13u},
            {'E', 14u},{'F', 15u},
        }};
        for (const auto& [k, v]: lut) { 
            if (k == c) return v;
        }

        return 0;
    }

    template <typename Str>
    constexpr color4 str_to_rgba(const Str& str) {
        assert(str.size() == 9 && "invalid rgba size");
        
        for (const auto c: str) {
            assert(is_hex_digit(c) || c == '#');
        }

        color4 res;
        res.x = f32(hex_value(str[1]) * 16u + hex_value(str[2])) / 255.0f;
        res.y = f32(hex_value(str[3]) * 16u + hex_value(str[4])) / 255.0f;
        res.z = f32(hex_value(str[5]) * 16u + hex_value(str[6])) / 255.0f;
        res.w = f32(hex_value(str[7]) * 16u + hex_value(str[8])) / 255.0f;
        return res;
    }

    constexpr color32 operator"" _rgba(const char* str, std::size_t size) {
        auto c = str_to_rgba(std::string_view(str, size));
        return to_color32(c);
    }

    constexpr color32 operator"" _abgr(const char* str, std::size_t size) {
        auto c = str_to_rgba(std::string_view(str, size));
        u32 res = 0u;
        res = 
            (u8(c.x * 255.0f) << 24) |
            (u8(c.y * 255.0f) << 16) |
            (u8(c.z * 255.0f) << 8)  |
            (u8(c.w * 255.0f) << 0)  ;

        return res; 
    }

    constexpr color4 operator"" _c4(const char* str, std::size_t size) {
        return str_to_rgba(std::string_view(str, size));
    }

    constexpr color3 operator"" _c3(const char* str, std::size_t size) {
        return color3{str_to_rgba(std::string_view(str, size))};
    }

    namespace rgba {
        static auto clear = "#00000000"_rgba;
        static auto black = "#000000ff"_rgba;
        static auto white = "#ffffffff"_rgba;
        static auto red   = "#ff0000ff"_rgba;
        static auto green = "#00ff00ff"_rgba;
        static auto blue  = "#0000ffff"_rgba;
        static auto yellow= "#ffff00ff"_rgba;
        static auto sand  = "#C2B280ff"_rgba;
    };

    namespace abgr {
        static auto clear = "#00000000"_abgr;
        static auto black = "#000000ff"_abgr;
        static auto white = "#ffffffff"_abgr;
        static auto red   = "#ff0000ff"_abgr;
        static auto green = "#00ff00ff"_abgr;
        static auto blue  = "#0000ffff"_abgr;
        static auto yellow= "#ffff00ff"_abgr;
        static auto sand  = "#C2B280ff"_abgr;
    };

    namespace v4 {
        static auto clear = "#00000000"_c4;
        static auto black = "#000000ff"_c4;
        static auto white = "#ffffffff"_c4;
        static auto ray_white   = "#f1f1f1ff"_c4;
        static auto light_gray  = "#d3d3d3ff"_c4;
        static auto dark_gray   = "#2a2a2aff"_c4;
        static auto red   = "#ff0000ff"_c4;
        static auto green = "#00ff00ff"_c4;
        static auto blue  = "#0000ffff"_c4;
        static auto purple= "#ff00ffff"_c4;
        static auto cyan  = "#00ffffff"_c4;
        static auto yellow= "#ffff00ff"_c4;
        static auto sand  = "#C2B280ff"_c4;
    };

    namespace v3 {
        static auto clear = "#00000000"_c3;
        static auto black = "#000000ff"_c3;
        static auto white = "#ffffffff"_c3;
        static auto gray  = "#333333ff"_c3;
        static auto red   = "#ff0000ff"_c3;
        static auto green = "#00ff00ff"_c3;
        static auto cyan  = "#00ffffff"_c3;
        static auto blue  = "#0000ffff"_c3;
        static auto purple= "#ff00ffff"_c3;
        static auto yellow= "#ffff00ff"_c3;
        static auto sand  = "#C2B280ff"_c3;
    };
};



