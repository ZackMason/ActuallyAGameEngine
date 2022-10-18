// Author: Zackery Mason-Blaug
// Date: 2022-10-13
//////////////////////////////////////////////////////////


#include "Util/console.hpp"

#include "glfw/glfw3.h"

#include "Engine/window.hpp"


namespace internal {
    console_t* console{nullptr};
};

console_t& console_t::get() {
    if (!internal::console) {
        internal::console = new console_t;
    }
    return *internal::console;
}

bool console_t::on_key_event(const key_event_t& e) {
    switch(e.key) {
        
        case GLFW_KEY_BACKSPACE:
            if (e.mode != 1) return false;
            if (is_open()) {
                if (input_text.empty() == false) {
                    input_text.pop_back();
                }
            }
            break;
        case GLFW_KEY_LEFT_BRACKET:
            if (is_open()) {
                scroll.y += 10;
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            if (is_open()) {
                scroll.y -= 10;
            }
            break;

        default:
            return false;
    }
    return true;
}