// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#include "Engine/ui_layer.hpp"

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ui_layer_t::draw() {
    ImGui::Begin("Debug");
    ImGui::Text("Fps: %f", ImGui::GetIO().Framerate);

    ImGui::End();

    ImGui::Begin("Entities");
    ImGui::Text("Fps: %f", ImGui::GetIO().Framerate);

    ImGui::End();
}

void ui_layer_t::new_frame() {
}

void ui_layer_t::render() {

}
