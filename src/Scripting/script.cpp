// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////



#include "Scripting/script.hpp"

#include "imgui.h"

#include "core.hpp"

#include <filesystem>

#include <entt/entt.hpp>
#include <fmt/core.h>

#include "Math/transform.hpp"

#include "Graphics/framebuffer.hpp"
#include "Graphics/screen.hpp"

#include "Util/random.hpp"
#include "Util/logger.hpp"

#include "GLFW/glfw3.h"


void script_vm_t::eval_file(const std::string& f, const std::string& asset_dir) {
    chai.eval_file(fmt::format("{}{}", asset_dir, f));
}

void script_vm_t::init(window_t& window, entt::registry& world, asset_loader_t& loader) {
    if (initialized) return;
    initialized = true;

    chai.add(chaiscript::fun(rand), "rand");

    chai.add(chaiscript::fun([](const std::string& s){
        logger_t::info(s);
    }), "print");
    chai.add(chaiscript::fun([](){return 1.0f/ImGui::GetIO().Framerate;}), "delta_time" );
    chai.add(chaiscript::fun([](){return random_s::randf();}), "randf");
    chai.add(chaiscript::fun([](){return random_s::randn();}), "randn");
    chai.add(chaiscript::fun(cosf), "cos");
    chai.add(chaiscript::fun(sinf), "sin");
    chai.add(chaiscript::fun([](){glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);}), "clear");
    chai.add(chaiscript::fun([](){glClear(GL_COLOR_BUFFER_BIT);}), "clear_color");
    chai.add(chaiscript::fun([](const v3f& a, const v3f& b, f32 lambda, f32 dt){return damp(a,b,lambda, dt);}), "damp");
    chai.add(chaiscript::fun([](const v3f& a, const v3f& b, f32 lambda, f32 dt){return lerp_dt(a,b,lambda, dt);}), "lerp_dt");
    chai.add(chaiscript::fun([](const v3f& a, const v3f& b, f32 lambda){return lerp(a,b,lambda);}), "lerp");
    chai.add(chaiscript::fun([](f32 a, f32 b, f32 lambda, f32 dt){return damp(a,b,lambda, dt);}), "damp");
    chai.add(chaiscript::fun([](f32 a, f32 b, f32 lambda, f32 dt){return lerp_dt(a,b,lambda, dt);}), "lerp_dt");
    chai.add(chaiscript::fun([](f32 a, f32 b, f32 lambda){return lerp(a,b,lambda);}), "lerp");
    
    chai.add(chaiscript::fun(glViewport), "glViewport");

    chai.add(chaiscript::user_type<glm::quat>(), "quat");
    chai.add(chaiscript::fun(&glm::quat::x), "x");
    chai.add(chaiscript::fun(&glm::quat::y), "y");
    chai.add(chaiscript::fun(&glm::quat::z), "z");
    chai.add(chaiscript::fun(&glm::quat::w), "w");

    chai.add(chaiscript::user_type<v3f>(), "v3f");
    chai.add(chaiscript::constructor<v3f()>(), "v3f");
    chai.add(chaiscript::constructor<v3f(f32,f32,f32)>(), "v3f");
    chai.add(chaiscript::constructor<v3f(const v3f&)>(), "v3f");
    chai.add(chaiscript::fun([](v3f& a, const v3f& b){a+=b; return a;}), "+=");
    chai.add(chaiscript::fun([](v3f& a, const v3f& b){a-=b; return a;}), "-=");
    chai.add(chaiscript::fun([](v3f& a, const v3f& b){a*=b; return a;}), "*=");
    chai.add(chaiscript::fun([](v3f& a, const v3f& b){a/=b; return a;}), "/=");
    
    chai.add(chaiscript::fun([](v3f& a, const f32 b){a+=b; return a;}), "+=");
    chai.add(chaiscript::fun([](v3f& a, const f32 b){a-=b; return a;}), "-=");
    chai.add(chaiscript::fun([](v3f& a, const f32 b){a*=b; return a;}), "*=");
    chai.add(chaiscript::fun([](v3f& a, const f32 b){a/=b; return a;}), "/=");

    chai.add(chaiscript::fun(&v3f::x), "x");
    chai.add(chaiscript::fun(&v3f::y), "y");
    chai.add(chaiscript::fun(&v3f::z), "z");
    chai.add(chaiscript::user_type<m33>(), "m33");
    chai.add(chaiscript::user_type<m44>(), "m44");

    chai.add(chaiscript::user_type<vertex_array_t>(), "vertex_array_t");
    chai.add(chaiscript::user_type<buffer_t<v3f>>(), "vec3_buffer_t");

    chai.add(chaiscript::fun(&buffer_t<v3f>::bind), "bind");
    chai.add(chaiscript::fun(&buffer_t<v3f>::unbind), "unbind");
    chai.add(chaiscript::fun(&buffer_t<v3f>::create), "create");
    chai.add(chaiscript::fun(&buffer_t<v3f>::data), "data");
    chai.add(chaiscript::fun(&vertex_array_t::bind), "bind");
    chai.add(chaiscript::fun(&vertex_array_t::unbind), "unbind");
    chai.add(chaiscript::fun(&vertex_array_t::draw), "draw");
    chai.add(chaiscript::fun(&vertex_array_t::set_attrib), "set_attrib");

    chai.add(chaiscript::user_type<asset_loader_t>(), "asset_loader_t");
    chai.add(chaiscript::user_type<resource_handle_t<shader_t>>(), "shader_handle_t");
    chai.add(chaiscript::user_type<resource_handle_t<static_mesh_t>>(), "static_mesh_handle_t");
    chai.add(chaiscript::user_type<resource_handle_t<texture2d_t>>(), "texture2d_handle_t");
    chai.add(chaiscript::user_type<resource_handle_t<framebuffer_t>>(), "framebuffer_handle_t");

    chai.add(chaiscript::fun(&resource_handle_t<shader_t>::get), "get");
    chai.add(chaiscript::fun(&resource_handle_t<static_mesh_t>::get), "get");
    chai.add(chaiscript::fun(&resource_handle_t<texture2d_t>::get), "get");
    chai.add(chaiscript::fun(&resource_handle_t<framebuffer_t>::get), "get");
    
    chai.add(chaiscript::fun(&asset_loader_t::get_shader_nameless), "get_shader");
    chai.add(chaiscript::fun(&asset_loader_t::get_shader_vs_fs), "get_shader");
    chai.add(chaiscript::fun(&asset_loader_t::get_texture2d), "get_texture2d");
    chai.add(chaiscript::fun(&asset_loader_t::create_texture2d), "create_texture2d");
    chai.add(chaiscript::fun(&asset_loader_t::get_static_mesh), "get_static_mesh");
    chai.add(chaiscript::fun(&asset_loader_t::get_framebuffer), "get_framebuffer");
    
    chai.add(chaiscript::fun(&texture2d_t::set_data), "set_data");
    chai.add(chaiscript::fun(&texture2d_t::set), "set");
    chai.add(chaiscript::fun(&texture2d_t::mipmap), "mipmap");
    chai.add(chaiscript::fun(&texture2d_t::set_filter), "set_filter");
    chai.add(chaiscript::fun(&texture2d_t::set_wrap), "set_wrap");
    
    chai.add(chaiscript::vector_conversion<std::vector<u32>>());
    chai.add(chaiscript::vector_conversion<std::vector<int>>());

    chai.add(chaiscript::fun(&shader_t::bind), "bind");
    chai.add(chaiscript::fun(&shader_t::set_int), "set_int");
    chai.add(chaiscript::fun(&shader_t::set_float), "set_float");
    chai.add(chaiscript::fun(static_cast<void(shader_t::*)(const std::string&, const v3f&)const>(&shader_t::set_vec3)), "set_vec3");
    chai.add(chaiscript::fun(static_cast<void(shader_t::*)(const std::string&, f32, f32, f32)const>(&shader_t::set_vec3)), "set_vec3");

    chai.add(chaiscript::user_type<framebuffer_t>(), "framebuffer_t");
    chai.add(chaiscript::constructor<framebuffer_t(int, int, bool)>(), "framebuffer_t");
    chai.add(chaiscript::fun([](framebuffer_t& f, u32 m){f.mode = (framebuffer_t::target_e) m;}), "mode");
    chai.add(chaiscript::fun(&framebuffer_t::bind), "bind");
    chai.add(chaiscript::fun(&framebuffer_t::slot), "slot");
    chai.add(chaiscript::fun(&framebuffer_t::resize), "resize");
    chai.add(chaiscript::fun(&framebuffer_t::blit), "blit");
    chai.add(chaiscript::fun(&framebuffer_t::unbind), "unbind");
    chai.add(chaiscript::fun(&framebuffer_t::mipmap), "mipmap");
    chai.add(chaiscript::fun(&framebuffer_t::width), "width");
    chai.add(chaiscript::fun(&framebuffer_t::height), "height");
    
    chai.add(chaiscript::user_type<screen_t>(), "screen_t");
    chai.add(chaiscript::constructor<screen_t()>(), "screen_t");
    chai.add(chaiscript::fun(&screen_t::draw), "draw");
    
    chai.add(chaiscript::user_type<transform_t>(), "transform_t");
    chai.add(chaiscript::constructor<transform_t()>(), "transform_t");
    chai.add(chaiscript::fun(&transform_t::origin), "origin");
    chai.add(chaiscript::fun(&transform_t::basis), "basis");
    chai.add(chaiscript::fun(&transform_t::set_position), "set_position");
    chai.add(chaiscript::fun(&transform_t::set_scale), "set_scale");
    chai.add(chaiscript::fun(&transform_t::translate), "translate");
    chai.add(chaiscript::fun(&transform_t::rotate), "rotate");
    chai.add(chaiscript::fun(&transform_t::xform), "xform");

    chai.add(chaiscript::user_type<entt::entity>(), "entt");
    chai.add(chaiscript::fun([](entt::registry& world){return world.create();}), "create");
    chai.add(chaiscript::user_type<entt::registry>(), "registry");
    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e) -> transform_t& {
        return reg.get<transform_t>(e);}), "entt_transform");
    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e) {
        return reg.try_get<transform_t>(e);}), "try_transform");

    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e){
        return reg.emplace<transform_t>(e);}), "add_transform");
    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e, const std::string& path){
        return reg.emplace<script_t>(e, path);}), "add_script");

    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e, resource_handle_t<shader_t>& res){
        return reg.emplace<resource_handle_t<shader_t>>(e, res);}), "add_shader");
    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e, resource_handle_t<static_mesh_t>& res){
        return reg.emplace<resource_handle_t<static_mesh_t>>(e, res);}), "add_static_mesh");
    chai.add(chaiscript::fun([](entt::registry& reg, entt::entity e, resource_handle_t<texture2d_t>& res){
        return reg.emplace<resource_handle_t<texture2d_t>>(e, res);}), "add_texture2d");



    chai.add_global(chaiscript::var(&window.width), "window_width");
    chai.add_global(chaiscript::var(&window.height), "window_height");
    chai.add_global(chaiscript::var(&world), "world");
    chai.add_global(chaiscript::var(&loader), "loader");
    chai.add(chaiscript::fun([&window](){window.close_window();}), "quit");

    

    chai.add(chaiscript::fun([&window](int key){return window.is_key_pressed(key);}), "is_key_pressed");
    chai.add(chaiscript::fun([&window](int key){return window.is_key_released(key);}), "is_key_released");

#if _DEBUG
    chai.add_global(chaiscript::var(true), "DEBUG");
#else
    chai.add_global(chaiscript::var(false), "DEBUG");
#endif

#if _WIN32
    chai.add_global(chaiscript::var(true), "WIN32");
#else
    chai.add_global(chaiscript::var(false), "WIN32");
#endif


}

void script_t::load_function(chaiscript::ChaiScript& chai)
{
    init = true;
    std::filesystem::path p(filename);
    std::string class_name = p.stem().string();

    try {
        chai.use(filename);
        script_object = chai.eval<chai_ptr_t>(class_name + "()");

        on_update = chai.eval<std::function<void (chai_ptr_t, entt::entity& e, const f32)>>("on_update");
    }  catch (chaiscript::exception::eval_error &e) {
        logger_t::chai_exception(e.pretty_print());
    } catch (chaiscript::exception::dispatch_error &e) {
        logger_t::chai_exception(e.what());
    } catch (std::exception & e) {
        logger_t::exception(e.what());
    }
}

