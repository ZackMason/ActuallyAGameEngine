// Author: Zackery Mason-Blaug
// Date: 2022-10-12

#pragma once

#include "types.hpp"

#include "Util/string.hpp"

#include "Math/aabb.hpp"

#include "Graphics/texture2d.hpp"

#include "Engine/resource_handle.hpp"

#include <functional>

struct font_t {
    struct glyph_t {
        char c{};
        aabb_t<v2f> screen{};
        aabb_t<v2f> texture{};
    };

    sid_t name{0};
private:
    f32 size{12};
public:
    void set_size(float s);

    glyph_t get_glyph(float x, float y, char c) const;

    v2f get_text_size(const std::string& text) const;

    void draw(const std::string& text, float x, float y, 
        std::function<void(resource_handle_t<texture2d_t>, const aabb_t<v2f>&, const aabb_t<v2f>&)> draw_fn);

    explicit font_t(const std::string& path, float s = 32.0f);
};
