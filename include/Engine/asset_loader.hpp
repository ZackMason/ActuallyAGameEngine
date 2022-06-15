// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

#include "Engine/heightmap.hpp"

#include "Graphics/texture2d.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/static_mesh.hpp"
#include "Graphics/framebuffer.hpp"

#include "Util/exceptions.hpp"

// idk about this.....
template <typename T>
struct resource_handle_t {
    resource_handle_t(T& p_res, u32& p_count) 
    : resource(p_res), count(p_count) 
    {
    }
    ~resource_handle_t() {
        count--;
    }

    resource_handle_t(resource_handle_t& o) 
    : resource(o.resource), count(o.count) 
    {
        count++;
    }
    resource_handle_t(resource_handle_t&& o)
        : resource(o.resource), count(o.count) 
    {
        count++;
    }
    resource_handle_t& operator=(const resource_handle_t& o) {
        resource = o.resource;
        count++;
        return *this;
    };
    resource_handle_t& operator=(resource_handle_t&& o) {
        resource = std::move(o.resource);
        count++;
        return *this;
    };
    operator int() {
        return resource.id;
    }
    T& get() const { return resource.get(); }

    const int get_count() const {
        return count;
    } 

private:
    u32& count;
    std::reference_wrapper<T> resource;
};

struct asset_loader_t {

    template<typename R>  
    struct cache_resource_t {
        cache_resource_t() = default;
        cache_resource_t(R* p, u32 c) :
            resource(p), count(c)
        {
            fmt::print(fg(fmt::color::orange) | fmt::emphasis::underline,
                "asset_loader_t: Allocating resource {}\n", reinterpret_cast<void*>(resource));
        }
        ~cache_resource_t() {
            if (count == 0) {
                fmt::print(fg(fmt::color::orange) | fmt::emphasis::underline,
                    "asset_loader_t: Freeing resource {}\n", reinterpret_cast<void*>(resource));
                resource->~R();
                free(resource);
            }
        }
        R *resource{nullptr};
        u32 count{0};
    };
    
    template <typename R>
    cache_resource_t<R> create_cache_resource() {
        return cache_resource_t<R>(reinterpret_cast<R*>(malloc(sizeof(R))), 1);
    }

    std::string asset_dir = ASSETS_PATH;

    std::unordered_map<std::string, cache_resource_t<static_mesh_t>> static_mesh_cache;
    std::unordered_map<std::string, cache_resource_t<texture2d_t>> texture2d_cache;
    std::unordered_map<std::string, cache_resource_t<shader_t>> shader_cache;
    std::unordered_map<std::string, cache_resource_t<framebuffer_t>> framebuffer_cache;
    std::unordered_map<std::string, utl::vector<f32>> heightmap_cache;

    template <typename T>
    void reload() {
        if(std::is_same<T, static_mesh_t>::value){ 
            throw not_implemented_x();
        }
        else if(std::is_same<T, framebuffer_t>::value){ 
            throw not_implemented_x();
        }
        else if(std::is_same<T, texture2d_t>::value){ 
            throw not_implemented_x();
        }
        else if(std::is_same<T, shader_t>::value){
            //throw not_implemented_x();
            for (const auto& [k, v] : shader_cache) {
                const auto files = v.resource->files;
                v.resource->stages.clear();
                v.resource->~shader_t();
                new (v.resource) shader_t(k, files, asset_dir);
            }
        }
        //return 0;
    }

    template <typename T>
    u32 get_count(const std::string& path){
        if(std::is_same<T, static_mesh_t>::value){ 
            return get_mesh_count(path);
        }
        else if(std::is_same<T, texture2d_t>::value){ 
            return get_tex2d_count(path);
        }
        else if(std::is_same<T, shader_t>::value){
            return get_shader_count(path);
        }
        return 0;
    }
    u32 get_shader_count(const std::string& name) {
        if (shader_cache.count(name)) { 
            const auto count = shader_cache[name].count;
            if (count == 0) {
                shader_cache.erase(name);
                return 0;
            }
            return count;
        }
        return 0;
    }
    u32 get_tex2d_count(const std::string& path) {
        if (texture2d_cache.count(path)) { 
            const auto count = texture2d_cache[path].count;
            if (count == 0) {
                texture2d_cache.erase(path);
                return 0;
            }
            return count;
        }
        return 0;
    }
    u32 get_mesh_count(const std::string& path) {
        if (static_mesh_cache.count(path)) { 
            const auto count = static_mesh_cache[path].count;
            if (count == 0) {
                static_mesh_cache.erase(path);
                return 0;
            }
            return count;
        }
        return 0;
    }

    utl::vector<f32> get_heightmap(const std::string& path) {
        if (heightmap_cache.count(path)) { 
            auto& heightmap = heightmap_cache[path];
            return heightmap;
        }
        get_heightmap_mesh(path);
        return get_heightmap(path);
    }
    resource_handle_t<static_mesh_t> get_heightmap_mesh(const std::string& path) {
        if (static_mesh_cache.count(path)) { 
            auto& [mesh, count] = static_mesh_cache[path];
            if (count == 0){
                static_mesh_cache.erase(path);
                return get_static_mesh(path);
            }
            return resource_handle_t(*mesh, ++count);
        }

        static_mesh_cache[path] = create_cache_resource<static_mesh_t>();
        auto& [mesh, count] = static_mesh_cache[path];
        auto [vertices, heightmap] = heightmap_t::load_vertices(this, path, 700.0f, 2.0f, 2.0f);
        new (mesh) static_mesh_t(std::move(vertices));
        heightmap_cache[path] = heightmap;
        mesh->update_aabb();
        return resource_handle_t(*mesh, count);
    }


    resource_handle_t<static_mesh_t> get_static_mesh(const std::string& path) {
        if (static_mesh_cache.count(path)) { 
            auto& [mesh, count] = static_mesh_cache[path];
            if (count == 0){
                static_mesh_cache.erase(path);
                return get_static_mesh(path);
            }
            return resource_handle_t(*mesh, ++count);
        }

        static_mesh_cache[path] = create_cache_resource<static_mesh_t>();
        static_mesh_t::emplace_obj(path, static_mesh_cache[path].resource, asset_dir);
        auto& [mesh, count] = static_mesh_cache[path];
        mesh->update_aabb();
        return resource_handle_t(*mesh, count);
    }

#if 1
    resource_handle_t<texture2d_t> create_texture2d(const std::string& name, u32 w, u32 h){
        if (texture2d_cache.count(name)) { 
            auto& [texture, count] = texture2d_cache[name];
            return resource_handle_t(*texture, ++count);
        }
        texture2d_cache[name] = create_cache_resource<texture2d_t>();
        new (texture2d_cache[name].resource) texture2d_t(w, h);
        auto& [texture, count] = texture2d_cache[name];
        return resource_handle_t(*texture, count);
    }

    resource_handle_t<texture2d_t> get_texture2d(const std::string& path){
        if (texture2d_cache.count(path)) { 
            auto& [texture, count] = texture2d_cache[path];
            return resource_handle_t(*texture, ++count);
        }
        texture2d_cache[path] = create_cache_resource<texture2d_t>();
        new (texture2d_cache[path].resource) texture2d_t(path, asset_dir);
        auto& [texture, count] = texture2d_cache[path];
        return resource_handle_t(*texture, count);
    }
#endif
    resource_handle_t<shader_t> get_shader_nameless(const std::string& vs, const std::string& fs){
        return get_shader(fmt::format("{}/{}",vs,fs), {vs, fs});
    }
    resource_handle_t<shader_t> get_shader_vs_fs(const std::string& name, const std::string& vs, const std::string& fs){
        return get_shader(name, {vs, fs});
    }
    resource_handle_t<shader_t> get_shader(const std::string& name, const utl::vector<std::string>& path){
        if (shader_cache.count(name)) { 
            auto& [shader, count] = shader_cache[name];
            return resource_handle_t(*shader, ++count);
        }
        shader_cache[name] = create_cache_resource<shader_t>();
        new (shader_cache[name].resource) shader_t(name, path, asset_dir);
        auto& [shader, count] = shader_cache[name];
        return resource_handle_t(*shader, count);
    }
    resource_handle_t<framebuffer_t> get_framebuffer(const std::string& name, int w, int h, bool msaa = false){
        if (framebuffer_cache.count(name)) { 
            auto& [framebuffer, count] = framebuffer_cache[name];
            return resource_handle_t(*framebuffer, ++count);
        }
        framebuffer_cache[name] = create_cache_resource<framebuffer_t>();
        new (framebuffer_cache[name].resource) framebuffer_t(w, h, msaa);
        auto& [framebuffer, count] = framebuffer_cache[name];
        return resource_handle_t(*framebuffer, count);
    }
};

