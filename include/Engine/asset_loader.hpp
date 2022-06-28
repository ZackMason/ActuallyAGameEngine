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
#include "Util/logger.hpp"

// idk about this.....
template <typename T>
struct resource_handle_t {
private:
    static T& get_dummy() {
        static T dummy;
        return dummy;
    }

    static u32& get_dummy_count() {
        static u32 dummy_count{0};
        return dummy_count;
    }
public:
    resource_handle_t() : resource(get_dummy()), count(get_dummy_count()) {
    }


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

    resource_handle_t<static_mesh_t> get_static_mesh(const std::string& path);
    resource_handle_t<texture2d_t> create_texture2d(const std::string& name, u32 w, u32 h);

    resource_handle_t<texture2d_t> get_texture2d(const std::string& path);
    resource_handle_t<shader_t> get_shader_nameless(const std::string& vs, const std::string& fs);
    resource_handle_t<shader_t> get_shader_vs_fs(const std::string& name, const std::string& vs, const std::string& fs);
    resource_handle_t<shader_t> get_shader(const std::string& name, const utl::vector<std::string>& path);
    resource_handle_t<framebuffer_t> get_framebuffer(const std::string& name, int w, int h, bool msaa = false);
};

