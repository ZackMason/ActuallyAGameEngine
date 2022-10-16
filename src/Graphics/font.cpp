// Author: Zackery Mason-Blaug
// Date: 2022-10-12

#include "Graphics\font.hpp"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include "vendor/stb_truetype.h"

#include <unordered_map>

#include "Engine/resource_handle.hpp"

#include "Util/allocators.hpp"
#include "Util/logger.hpp"

#include "Graphics/texture2d.hpp"

namespace internal {
    bool initialized{false};
stack_allocator_t font_allocator{1024<<16};
    unsigned char* ttf_buffer;
    unsigned char* temp_bitmap;
    stbtt_bakedchar cdata[96];
    
    resource_handle_t<texture2d_t> font_texture;

    // true on success
    bool init() {
        static u32 c = 0;
        auto p_texture = font_allocator.alloc<texture2d_t>(1);

        new (p_texture) texture2d_t(512, 512, GL_R8, GL_RED, GL_UNSIGNED_BYTE);
        font_texture = resource_handle_t<texture2d_t>(p_texture, c);

        return true;
    }

    void lazy_init() {
        if (!initialized) {
            initialized = init();
        }
    }

    sid_t load_font(const std::string& font_path, float size) {
        const sid_t id = save_string(font_path);
        
        const auto stack_mark = font_allocator.get_marker();
        ttf_buffer = font_allocator.alloc<unsigned char>(1<<20);
        temp_bitmap = font_allocator.alloc<unsigned char>(512*512);
        
        const auto file = fopen(font_path.c_str(), "rb");

        fread(ttf_buffer, 1, 1<<20, file);
        stbtt_BakeFontBitmap(ttf_buffer, 0, size, temp_bitmap, 512, 512, 32, 96, cdata);

        glTextureSubImage2D(font_texture.get().id, 0, 0, 0, 512, 512, GL_RED, GL_UNSIGNED_BYTE, temp_bitmap);
        
        fclose(file);

        font_allocator.pop(stack_mark);

        return id;
    }
};

font_t::font_t(const std::string& path, float s) 
    : size(s) 
{
    internal::lazy_init();

    name = internal::load_font(path, size);
}

v2f font_t::get_text_size(const std::string& text) const {
    v2f size{};
    for (const auto c: text) {
        if (c > 32 && c < 128) {
            const auto g = get_glyph(0, 0, c);
            size.x += g.screen.size().x + 1;
            size.y = size.y > g.screen.size().y ? size.y : g.screen.size().y;
        } else if (c == ' ') { size.x += 3; }
    }
    return size;
}

void font_t::set_size(float s) {
    size = s;
    name = internal::load_font(sid_to_string(name), size);
}

void font_t::draw(const std::string& text, float x, float y, 
    std::function<void(resource_handle_t<texture2d_t>, const aabb_t<v2f>&, const aabb_t<v2f>&)> draw_fn)
{
    v2f current_pos{x,y};
    for (const auto c: text) {
        if (c > 32 && c < 128) {
            auto glyph = get_glyph(current_pos.x, current_pos.y, c);

            draw_fn(internal::font_texture, glyph.screen, glyph.texture);
            current_pos.x += glyph.screen.size().x + 1;
        } else if (c == ' ') { current_pos.x += 5; }
    }
}

font_t::glyph_t font_t::get_glyph(float x, float y, char c) const {
    aabb_t<v2f> screen{};
    aabb_t<v2f> texture{};

    stbtt_aligned_quad q;
    stbtt_GetBakedQuad(internal::cdata, 512, 512, c - 32, &x,&y, &q, 1);

    texture.expand(v2f{q.s0, q.t0});
    texture.expand(v2f{q.s1, q.t1});

    screen.expand(v2f{q.x0, q.y0});
    screen.expand(v2f{q.x1, q.y1});

    return glyph_t {
        c, 
        screen,
        texture
    };
}
