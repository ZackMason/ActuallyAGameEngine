// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include "Event/event.hpp"

#include <array>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#if defined(CreateWindow)
#undef CreateWindow
#endif

enum struct gfx_api_e {
    OPENGL, DX12, SIZE
};

struct GLFWwindow;
struct window_t {
    u32 width{640}, height{480};
    std::array<f32, 2> scroll;

   	using window_event_callback = std::function<void(event_i&)>;
    window_event_callback event_callback;

    void set_event_callback(window_event_callback&& callback) {
        event_callback = callback;
    }

    ~window_t();
    void open_window(gfx_api_e api = gfx_api_e::OPENGL);

    HWND get_handle() const;

    void set_width(int);
    void set_height(int);

    v2f size() const {
        return v2f{static_cast<f32>(width), static_cast<f32>(height)};
    }

    void set_fullscreen(bool full);
    void toggle_fullscreen();
    void set_vsync(bool vsync);

    void make_imgui_context() const;
    void imgui_new_frame() const;
    void imgui_render() const;

    void viewport(int x, int y, int w, int h) const;

    void set_position(const v2i& pos);
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
    decltype(scroll) get_scroll();

    f32 get_ticks() const;

    GLFWwindow *window{nullptr};

    void* get_dx_state() const;
private:
    const char* title = "TheGame";
};

struct position2d_event_t : public v2i, public event_i {
    position2d_event_t(int px, int py) {
        x = px;
        y = py;
    }
    MAKE_EVENT_TYPE(position2d)
};

struct window_resize_event_t : position2d_event_t {
    using position2d_event_t::position2d_event_t;
    MAKE_EVENT_TYPE(window_resize)
};

struct mouse_move_event_t : position2d_event_t {
    using position2d_event_t::position2d_event_t;
    MAKE_EVENT_TYPE(mouse_move)
};

struct mouse_scroll_event_t : position2d_event_t {
    using position2d_event_t::position2d_event_t;
    MAKE_EVENT_TYPE(mouse_scroll)
};

struct file_dropped_event_t : event_i {
    explicit file_dropped_event_t() = default;
    explicit file_dropped_event_t(std::vector<std::string>&& files_) 
        : files(std::move(files_))
    {}
    virtual ~file_dropped_event_t() = default;

    std::vector<std::string> files;

    MAKE_EVENT_TYPE(file_dropped);
};

struct key_event_t : public event_i {
    explicit key_event_t(int pk, int ps, int pm) :
    key(pk), scancode(ps), mode(pm) {}

    int key;
    int scancode;
    int mode;

    MAKE_EVENT_TYPE(key);
};

struct char_event_t : public event_i {
    explicit char_event_t(unsigned int cp) :
        codepoint(cp) {}

    unsigned int codepoint;

    MAKE_EVENT_TYPE(char);
};  