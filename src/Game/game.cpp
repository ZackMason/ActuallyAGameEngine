// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#include "Game/game.hpp"

#include "core.hpp"

#include <execution>

#include <glad/glad.h>
#include <glfw/glfw3.h>

#include <fmt/core.h>
#include <fmt/color.h>

#include <entt/entt.hpp>

#include "imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Engine/ui_layer.hpp"

#include "Graphics/screen.hpp"
#include "Graphics/compute.hpp"
#include "Graphics/framebuffer.hpp"

#include "Math/transform.hpp"

struct sky_box_t {
    bool depth;
};

namespace internal {
};

void game_t::init() {
    ImGui_ImplGlfw_InitForOpenGL(window.window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    script_vm.init(window, world, asset_loader);

    //game_client.tick_rate = 0;
    //game_client.host_connect("127.0.0.1", 1234);
    //game_client.poll_events(1);

    glEnable(GL_DEPTH_TEST);  

    auto sky_shader = asset_loader.get_shader_nameless("shaders/skybox.vs", "shaders/skybox.fs");
    auto sky_mesh = asset_loader.get_static_mesh("models/skysphere.obj");

    auto sky = world.create();
    world.emplace<transform_t>(sky);
    world.emplace<sky_box_t>(sky);
    world.emplace<resource_handle_t<static_mesh_t>>(sky, sky_mesh);
    world.emplace<resource_handle_t<shader_t>>(sky, sky_shader);
        
    script_vm.eval_file("scripts/Startup.chai");
        
    glEnable(GL_CULL_FACE);    
    glCullFace(GL_BACK);
}
void game_t::pre_render_pass() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}
void game_t::main_render_pass() {

    auto shader = asset_loader.get_shader_nameless("shaders/base.vs", "shaders/texture.fs");
    shader.get().bind();
    shader.get().set_vec3("uCameraPos", camera.fly_camera.get_position());

    static pipeline_script_t msaa_pass{"scripts/MsaaPass.chai", [&](){

        auto sky_view = world.view<const transform_t, const sky_box_t,
            const resource_handle_t<static_mesh_t>, 
            const resource_handle_t<shader_t>>();

        sky_view.each([&](const transform_t& t, const sky_box_t& sb, 
            const resource_handle_t<static_mesh_t>& mesh, 
            const resource_handle_t<shader_t>& shader)  {
                shader.get().bind();
                shader.get().set_mat4("uM", t.to_matrix());
                shader.get().set_mat4("uVP", camera.view_projection());
                mesh.get().draw();
        });

        glClear(GL_DEPTH_BUFFER_BIT);

        auto view = world.view<const transform_t, 
            const resource_handle_t<static_mesh_t>, 
            const resource_handle_t<shader_t>>(
                entt::exclude<resource_handle_t<texture2d_t>, sky_box_t>
            );

        f32 i = 0.1f;

        view.each([&](const transform_t& t, 
            const resource_handle_t<static_mesh_t>& mesh, 
            const resource_handle_t<shader_t>& shader)  {
                shader.get().bind();
                shader.get().set_vec3("uColor", v3f{i, i, i});
                shader.get().set_mat4("uM", t.to_matrix());
                shader.get().set_mat4("uVP", camera.view_projection());
                i += 0.05f;
                mesh.get().draw();
        });

        auto texture_view = world.view<const transform_t, 
            const resource_handle_t<static_mesh_t>, 
            const resource_handle_t<shader_t>,
            const resource_handle_t<texture2d_t>>(entt::exclude<sky_box_t>);

        texture_view.each([&](const transform_t& t, 
            const resource_handle_t<static_mesh_t>& mesh, 
            const resource_handle_t<shader_t>& shader,
            const resource_handle_t<texture2d_t>& texture)  {
                shader.get().bind();
                shader.get().set_int("uTexture", 0);
                texture.get().bind_slot(0);
                texture.get().bind();
                shader.get().set_mat4("uM", t.to_matrix());
                shader.get().set_mat4("uVP", camera.view_projection());
                mesh.get().draw();
        });

    }};

    static pipeline_script_t post_process_pass{"scripts/PostProcess.chai", [](){}};

    static texture_compute_t compute{"shaders/blur.cs", (int)window.width, (int)window.height};


    msaa_pass.execute(1.0f / 60.0f, script_vm.chai);

    auto framebuffer = asset_loader.get_framebuffer("msaa_out", window.width, window.height, false);

    //framebuffer.get().unbind();
    //framebuffer.get().mode = framebuffer_t::target_e::TEXTURE;
    //for (int i = 0; i < 2; i++) {
    //    glBindImageTexture(0, framebuffer.get().tbo, i, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    //    compute.execute(0);    
    //    compute.output.slot = 0;
    //    compute.output.bind_image();
    //    glBindImageTexture(1, framebuffer.get().tbo, i, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA16F);
    //    glDispatchCompute(compute.output.width, compute.output.height, 1);
    //    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
    //}
    
    //framebuffer.get().slot = 0;
    //framebuffer.get().bind();


    post_process_pass.execute(1.0f / 60.0f, script_vm.chai);

    static bool p = true;
    if (window.is_key_pressed(GLFW_KEY_R)){
        if (p) {
            p = false;
            msaa_pass.reload(script_vm.chai);
            post_process_pass.reload(script_vm.chai);
            asset_loader.reload<shader_t>();
            new (&compute) texture_compute_t("shaders/blur.cs", (int)window.width, (int)window.height);
        }
    }else
        p = true;




    ImGui::Begin("compute");
        ImGui::Image((ImTextureID)framebuffer.get().tbo, {(f32)framebuffer.get().width/4,(f32)framebuffer.get().height/4});
    ImGui::End();
    
}

void game_t::post_process_pass() {
    static ui_layer_t ui;
    ui.draw();


    ImGui::Begin("Entities");

        ImGui::Text("Entities: %d", world.size());
        ImGui::Separator();

        int i = 0;
        world.each([&](auto entity){
            auto label = fmt::format("Entity: {}", (int)i++);
            if(ImGui::TreeNode(label.c_str())){
                if (auto t = world.try_get<transform_t>(entity)) {
                    ImGui::DragFloat3("origin",  &t->origin.x, 0.1f);
                    //ImGui::DragFloat3("origin",  &t->origin.x, 0.1f);
                }
                if (auto script = world.try_get<script_t>(entity)) {
                    ImGui::Text("Script: %s", script->filename.c_str());
                }
                if (auto texture = world.try_get<resource_handle_t<texture2d_t>>(entity)) {
                    ImGui::Text("Size: %d, %d", texture->get().width, texture->get().height);
                    if (ImGui::Button("Linear")) { texture->get().set_filter(true);}
                    ImGui::SameLine();
                    if (ImGui::Button("Nearest")) { texture->get().set_filter(false);}
                    if (ImGui::Button("Wrap")) { texture->get().set_wrap(true);}
                    ImGui::SameLine();
                    if (ImGui::Button("Clamp")) { texture->get().set_wrap(false);}
                }
                if (auto script = world.try_get<script_t>(entity)) {
                    ImGui::Text("Script: %s", script->filename.c_str());
                }
                ImGui::TreePop();
            }

        });

    ImGui::End();




    static bool open = true;

    //for (auto& message : game_client.message_queue)
    //    chat_log.AddLog(message.c_str());
    //game_client.message_queue.clear();

    //chat_log.Draw("Client Console", &open, game_client.peer, nullptr);

    //ui.render();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    shader_t::reset();    
    vertex_array_t::reset();    
}
void game_t::update() {
    //game_client.poll_events(1);

    camera.update(window, 1.0f / 60.0f);
    
    auto script_view = world.view<script_t>();
    if (!script_view.empty()) {
        std::for_each(std::execution::par, script_view.begin(), script_view.end(), 
            [&](entt::entity e) {
                auto& script = script_view.get<script_t>(e);
                if (!script.init) {
                    script.load_function(script_vm.chai);
                }
                script.on_update(script.script_object, e, 1.0f/60.0f);
        });
    }
    #if 0
    #endif

    auto camera_view = world.view<sky_box_t, transform_t>();
    std::for_each(std::execution::par, camera_view.begin(), camera_view.end(), 
        [&](auto e) {
            auto& transform = world.get<transform_t>(e);
            transform.set_position(camera.fly_camera.position);    
    });

}
void game_t::close() {
    //game_client.disconnect();
    //game_client.poll_events(1);
}