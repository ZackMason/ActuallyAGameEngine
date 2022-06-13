// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#undef near
#undef far

#include "Game/main_menu.hpp"

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void main_menu_t::init() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    
    ImGui::StyleColorsDark();
    
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

}
void main_menu_t::pre_render_pass()  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void main_menu_t::main_render_pass() {
    ImGui::Begin("Main Menu");
        if (ImGui::Button("Launch Server")){
            is_server = true;
            window.close_window();
        }
        if (ImGui::Button("Launch Client")){
            is_server = false;
            window.close_window();
        }

    ImGui::End();

}
void main_menu_t::post_process_pass() {

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void main_menu_t::update() {

}

void main_menu_t::close() {
    window.close_window();
}