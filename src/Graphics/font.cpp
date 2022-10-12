// Author: Zackery Mason-Blaug
// Date: 2022-10-12

#include "Graphics\font.hpp"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <unordered_map>

#include "Util/allocators.hpp"
#include "Util/logger.hpp"

namespace internal {
    bool initialized{false};
    FT_Library font_library;
    stack_allocator_t font_allocator{1024};
    std::unordered_map<sid_t, FT_Face*> loaded_fonts;

    // true on success
    bool init() {
        return FT_Init_FreeType(&font_library) == 0;
    }

    void lazy_init() {
        if (!initialized) {
            initialized = init();
        }
    }

    sid_t load_font(const std::string& font_path) {
        const sid_t id = save_string(font_path);
        
        if (loaded_fonts.count(id)) {
            logger_t::warn(fmt::format("Font is already loaded: {}", font_path));
        }

        loaded_fonts[id] = font_allocator.alloc<FT_Face>(1);

        if (FT_New_Face(font_library, font_path.c_str(), 0, loaded_fonts[id])) {
            logger_t::warn(fmt::format("Failed to load font: {}", font_path));
        }

        return id;
    }

    FT_Face* get_font(sid_t id) {
        return loaded_fonts.count(id) ? loaded_fonts[id] : nullptr;
    }
};

font_t::font_t(const std::string& path) {
    internal::lazy_init();

    name = internal::load_font(path);
}

font_t::glyph_t font_t::get_glyph(char c) const {
    FT_Face* font_face = internal::get_font(name);
    if (!font_face) {
        std::terminate();
    }   

    return glyph_t {
        c, 
        v2i{},
        v2i{}
    };
}
