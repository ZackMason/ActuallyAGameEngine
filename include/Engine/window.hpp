// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <array>

struct GLFWwindow;
struct window_t {
    u32 width{640}, height{480};
    std::array<f32, 2> scroll;

    ~window_t();

    void set_width(int);
    void set_height(int);

    void set_fullscreen(bool full);

    void open_window();
    void close_window();
    void swap_buffers();
    void poll_events();
    const bool should_close() const;
    const bool is_open() const;
    void set_title(const char* t);

    bool is_key_pressed(int key) const;
    bool is_key_released(int key) const;
    std::array<f32, 2> get_mouse() const;
    bool is_button_pressed(int button) const;
    decltype(scroll) get_scroll() const;

    GLFWwindow *window{nullptr};
private:
    const char* title = "TheGame";
};

