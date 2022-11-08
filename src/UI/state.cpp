// Author: Zackery Mason-Blaug
// Date: 2022-10-24
//////////////////////////////////////////////////////////


#include "UI/state.hpp"

#include "UI/panel.hpp"

#include <stack>

namespace zui {
    namespace internal {
        font_t* current_font{nullptr};
        std::stack<theme_t> theme_stack{};

        std::vector<panel_t> panels;
    };



    void set_font(font_t* font) {
        internal::current_font = font;
    }
    [[nodiscard]] font_t* get_font() {
        return internal::current_font;
    }

    [[nodiscard]] bool is_initialized() {
        return !!internal::current_font;
    }

    [[nodiscard]] theme_t get_theme() {
        return internal::theme_stack.top();
    }

    void pop_theme() {
        if (!internal::theme_stack.empty()) {
            internal::theme_stack.pop();            
        }
    }
    void push_theme(theme_t&& theme) {
        internal::theme_stack.emplace(std::move(theme));
    }
    
    bool start_panel(sid_t id) {
        panel_t panel;

        panel.id = id;

        return true;

    }
    void end_panel();

    bool button(sid_t id);
    bool checkbox(sid_t id, bool& x);
    bool int_slider(sid_t id, int& x);
    bool f32_slider(sid_t id, f32& x);
    bool text_box(sid_t id);
    void label(sid_t id);

};