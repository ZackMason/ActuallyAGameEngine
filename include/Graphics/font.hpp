// Author: Zackery Mason-Blaug
// Date: 2022-10-12

#pragma once

#include "types.hpp"

#include "Util/string.hpp"

struct font_t {
    struct glyph_t {
        char c{};
        v2i size{};
        v2i offset{};
    };

    sid_t name{0};
    u32 size{12};

    glyph_t get_glyph(char c) const;

    explicit font_t(const std::string& path);
};
