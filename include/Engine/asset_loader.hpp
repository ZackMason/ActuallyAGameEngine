// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include <fmt/core.h>
#include <fmt/color.h>


#include "Engine/heightmap.hpp"
#include "Engine/resource_handle.hpp"

#include "Graphics/texture2d.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/static_mesh.hpp"
#include "Graphics/framebuffer.hpp"
#include "Graphics/skeleton_animation.hpp"
#include "Graphics/skeletal_mesh.hpp"

#include "Util/exceptions.hpp"
#include "Util/logger.hpp"


struct asset_loader_t {
    template<typename R>  
    struct cache_resource_t {
        cache_resource_t() = default;
        cache_resource_t(R* p, u32 c) :
            resource(p), count(c)
        {
            logger_t::allocation(fmt::format("Allocating resource '{}' - {}", typeid(R).name(), reinterpret_cast<void*>(resource)));
        }
        ~cache_resource_t() {
            if (count == 0) {
                logger_t::allocation(fmt::format("Freeing resource {}", reinterpret_cast<void*>(resource)));
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

    template <typename Res>
    using cache_t = std::unordered_map<std::string, cache_resource_t<Res>>;

    cache_t<static_mesh_t> static_mesh_cache;
    cache_t<texture2d_t> texture2d_cache;
    cache_t<shader_t> shader_cache;
    cache_t<framebuffer_t> framebuffer_cache;
    cache_t<skeletal_model_t> skeletal_model_cache;
    cache_t<skeleton_animation_t> animation_cache;

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
    u32 get_shader_count(const std::string& name);
    u32 get_tex2d_count(const std::string& path);
    u32 get_mesh_count(const std::string& path);

    utl::vector<f32> get_heightmap(const std::string& path);
    resource_handle_t<static_mesh_t> get_heightmap_mesh(const std::string& path);

    bool has_static_mesh(const std::string& path);
    resource_handle_t<static_mesh_t> get_static_mesh(const std::string& path);
    resource_handle_t<static_mesh_t> create_static_mesh(const std::string& name, utl::vector<static_vertex_t>&& vertices);
    resource_handle_t<static_mesh_t> create_static_mesh(const std::string& name, utl::vector<static_vertex_t>&& vertices, utl::vector<unsigned int>&& indices);
    resource_handle_t<texture2d_t> create_texture2d(const std::string& name, u32 w, u32 h);

    resource_handle_t<texture2d_t> get_texture2d(const std::string& path);
    resource_handle_t<shader_t> get_shader_nameless(const std::string& vs, const std::string& fs);
    resource_handle_t<shader_t> get_shader_vs_fs(const std::string& name, const std::string& vs, const std::string& fs);
    resource_handle_t<shader_t> get_shader(const std::string& name, const utl::vector<std::string>& path);
    resource_handle_t<framebuffer_t> get_framebuffer(const std::string& name, int w, int h, bool msaa = false);
    resource_handle_t<skeletal_model_t> get_skeletal_model(const std::string& path);
    resource_handle_t<skeleton_animation_t> get_animation(const std::string& path, const std::string& model);
    resource_handle_t<skeleton_animation_t> get_sub_animation(const std::string& path, const std::string& anim,  const std::string& model);
};

