// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include "Engine/window.hpp"

#include <exception>
#include <stdexcept>

namespace internal {
    static bool glfw_initialized = false;
    static bool glad_initialized = false;

    void init_glfw() {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfw_initialized = true;

        atexit(glfwTerminate);
    }
};

window_t::~window_t(){
    if(window) {
        glfwDestroyWindow(window);
    }
}

void window_t::poll_events() {
    glfwPollEvents();            
}
void window_t::close_window() {
    glfwSetWindowShouldClose(window, true);
}

const bool window_t::is_open() const { 
    return window != nullptr && !glfwWindowShouldClose(window); 
}


void window_t::open_window() {
    if (!internal::glfw_initialized) internal::init_glfw();

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr){
        throw std::runtime_error("Failed to create glfw window");
    }
    glfwMakeContextCurrent(window);

    if (!internal::glad_initialized && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        throw std::runtime_error("Failed to load glad");
    }
    internal::glad_initialized = true;

    glfwSetWindowUserPointer(window, this);
    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow* window, int w, int h) {
            window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
            self.width = w;
            self.height = h;
            glViewport(0,0,w,h);
        });
    glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) {
            window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
            self.scroll = {(f32)x, (f32)y};
        });

}


decltype(window_t::scroll) window_t::get_scroll() const {
    return scroll;
}

std::array<f32, 2> window_t::get_mouse() const {
    double x,y;
    glfwGetCursorPos(window, &x, &y);
    return {static_cast<f32>(x), static_cast<f32>(y)};
}
bool window_t::is_button_pressed(int button) const {
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}
bool window_t::is_key_pressed(int key) const {
    return glfwGetKey(window, key) == GLFW_PRESS;
}
bool window_t::is_key_released(int key) const {
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

const bool window_t::should_close() const {
    return glfwWindowShouldClose(window);
}

void window_t::swap_buffers() {
    glfwSwapBuffers(window);
    scroll = {0.0f, 0.0f};
}