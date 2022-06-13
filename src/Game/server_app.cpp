// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#include "Game/server_app.hpp"

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "core.hpp"

void server_app_t::init() {
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    game_server.tick_rate = 0;
}

void server_app_t::pre_render_pass()  {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    glClear(GL_COLOR_BUFFER_BIT);
}
void server_app_t::main_render_pass() {
    ImGui::Begin("Server Menu");
        ImGui::Text(fmt::format("Players connected: {}", game_server.player_connected).c_str());
    ImGui::End();

    static bool open = true;
    chat_log.Draw("Console", &open, nullptr, &game_server);

}
void server_app_t::post_process_pass() {

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void server_app_t::update() {
    game_server.pump_event();
}

void server_app_t::close() {
    window.close_window();
}