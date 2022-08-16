// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Engine/window.hpp"

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <exception>
#include <stdexcept>

namespace internal {
    static bool glfw_initialized = false;
    static bool glad_initialized = false;
    static bool imgui_initialized = false;
    static GLFWwindow* imgui_window_context{nullptr};

    void init_glad() {
        if (!glad_initialized && !gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            throw std::runtime_error("Failed to load glad");
        }
        internal::glad_initialized = true;
    }

    void init_imgui(GLFWwindow* window) {
        IMGUI_CHECKVERSION();
        if (imgui_initialized) {
            ImGui::DestroyContext();
        }
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        
        ImGui::StyleColorsDark();
        
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 130");

        imgui_initialized = true;
        imgui_window_context = window;
    }

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

void window_t::set_position(const v2i& p) {
    glfwSetWindowPos(window, p.x, p.y);
}
void window_t::set_width(int w) {
    width = w;
    glfwSetWindowSize(window, width, height);
}
void window_t::set_height(int h) {
    height = h;
    glfwSetWindowSize(window, width, height);
}
void window_t::set_title(const char* t) {
    glfwSetWindowTitle(window, t);
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

void window_t::make_imgui_context() const {
    internal::init_imgui(window);
}

void window_t::imgui_new_frame() const {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void window_t::imgui_render() const {
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void window_t::open_window() {
    if (!internal::glfw_initialized) internal::init_glfw();

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (window == nullptr){
        throw std::runtime_error("Failed to create glfw window");
    }

    glfwMakeContextCurrent(window);

    internal::init_glad();
    
    glfwSetWindowUserPointer(window, this);
    
    glfwSetFramebufferSizeCallback(window,
        [](GLFWwindow* window, int w, int h) {
            if (!w||!h) return;
            window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));

            self.width = w;
            self.height = h;
            glViewport(0,0,w,h);
            window_resize_event_t event{w,h};
            self.event_callback(event);
    });
    
    glfwSetScrollCallback(window, [](GLFWwindow* window, double x, double y) {
        if (internal::imgui_window_context == window)
            if (ImGui::GetIO().WantCaptureMouse) return;
        window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
        self.scroll = {self.scroll[0] + (f32)x, self.scroll[1] + (f32)y};
        
        mouse_scroll_event_t event{(int)x, (int)y};
        self.event_callback(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double x, double y) {
        if (internal::imgui_window_context == window)
            if (ImGui::GetIO().WantCaptureMouse) return;
        window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
    
        mouse_move_event_t event{(int)x,(int)y};
        self.event_callback(event);

    });

    glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mode){
        if (internal::imgui_window_context == window)
            if (ImGui::GetIO().WantCaptureKeyboard) return;
        window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
    
        key_event_t event{key, scancode, action};
        self.event_callback(event);
    });

    glfwSetDropCallback(window, [](GLFWwindow* window, int count, const char** paths){
        window_t& self = *static_cast<window_t*>(glfwGetWindowUserPointer(window));
        
        std::vector<std::string> files;
        for (int i = 0; i < count; i++) {
            files.emplace_back(paths[i]);
        }
        file_dropped_event_t event{std::move(files)};
        self.event_callback(event);
    });
}

decltype(window_t::scroll) window_t::get_scroll() {
    const auto t = scroll;
    scroll = {0.0f, 0.0f};
    return t;
}

f32 window_t::get_ticks() const {
    return static_cast<f32>(glfwGetTime());
}

std::array<f32, 2> window_t::get_mouse() const {
    double x,y;
    glfwGetCursorPos(window, &x, &y);
    return {static_cast<f32>(x), static_cast<f32>(y)};
}

bool window_t::is_button_pressed(int button) const {
    if (ImGui::GetIO().WantCaptureMouse) return false;
    return glfwGetMouseButton(window, button) == GLFW_PRESS;
}

bool window_t::is_key_pressed(int key) const {
    if (ImGui::GetIO().WantCaptureKeyboard) return false;
    return glfwGetKey(window, key) == GLFW_PRESS;
}

bool window_t::is_key_released(int key) const {
    if (ImGui::GetIO().WantCaptureKeyboard) return true;
    return glfwGetKey(window, key) == GLFW_RELEASE;
}

void window_t::set_vsync(bool vsync) {
    glfwSwapInterval(vsync ? 60 : 0);
}

void window_t::toggle_fullscreen() {
    auto monitor = glfwGetWindowMonitor(window); // false if windowed mode
    set_fullscreen(!monitor);
}

void window_t::set_fullscreen(bool full) {
    glfwSetWindowMonitor(window, full ? glfwGetPrimaryMonitor() : nullptr, GLFW_DONT_CARE, GLFW_DONT_CARE, width, height, GLFW_DONT_CARE);
}

const bool window_t::should_close() const {
    return glfwWindowShouldClose(window);
}

void window_t::swap_buffers() {
    glfwSwapBuffers(window);
    scroll = {0.0f, 0.0f};
}
