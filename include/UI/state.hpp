// Author: Zackery Mason-Blaug
// Date: 2022-10-24
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/font.hpp"

#include "UI/theme.hpp"

namespace zui {
    void set_font(font_t* font);
    [[nodiscard]] font_t* get_font();

    [[nodiscard]] bool is_initialized();

    [[nodiscard]] theme_t get_theme();
    void pop_theme();
    void push_theme(theme_t&& theme);

    bool start_panel(sid_t id);
    void end_panel();

    bool button(sid_t id);
    bool checkbox(sid_t id, bool& x);
    bool int_slider(sid_t id, int& x);
    bool f32_slider(sid_t id, f32& x);
    bool text_box(sid_t id);
    void label(sid_t id);
};

