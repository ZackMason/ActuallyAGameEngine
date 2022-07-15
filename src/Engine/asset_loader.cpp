// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Engine/asset_loader.hpp"

#include "Graphics/skeletal_mesh.hpp"

u32 asset_loader_t::get_shader_count(const std::string& name) {
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
u32 asset_loader_t::get_tex2d_count(const std::string& path) {
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
u32 asset_loader_t::get_mesh_count(const std::string& path) {
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

utl::vector<f32> asset_loader_t::get_heightmap(const std::string& path) {
    if (heightmap_cache.count(path)) { 
        auto& heightmap = heightmap_cache[path];
        return heightmap;
    }
    get_heightmap_mesh(path);
    return get_heightmap(path);
}

resource_handle_t<static_mesh_t> asset_loader_t::get_heightmap_mesh(const std::string& path) {
    if (static_mesh_cache.count(path)) { 
        auto& [mesh, count] = static_mesh_cache[path];
        if (count == 0){
            static_mesh_cache.erase(path);
            return get_static_mesh(path);
        }
        return resource_handle_t(mesh, ++count);
    }

    static_mesh_cache[path] = create_cache_resource<static_mesh_t>();
    auto& [mesh, count] = static_mesh_cache[path];
    auto [vertices, indices, heightmap] = heightmap_t::load_vertices(this, path, 700.0f, 4.0f, 4.0f);
    new (mesh) static_mesh_t(std::move(indices), std::move(vertices));
    heightmap_cache[path] = heightmap;
    mesh->update_aabb();
    return resource_handle_t(mesh, count);
}

bool asset_loader_t::has_static_mesh(const std::string& name) {
    return static_mesh_cache.count(name) != 0;
}

resource_handle_t<static_mesh_t> asset_loader_t::create_static_mesh(
    const std::string& path, 
    utl::vector<static_vertex_t>&& vertices,
    utl::vector<unsigned>&& indices
) {
    if (static_mesh_cache.count(path)) { 
        auto& [mesh, count] = static_mesh_cache[path];
        return resource_handle_t(mesh, ++count);
    }
    static_mesh_cache[path] = create_cache_resource<static_mesh_t>();
    new (static_mesh_cache[path].resource) static_mesh_t(std::move(indices), std::move(vertices));
    auto& [mesh, count] = static_mesh_cache[path];
    mesh->update_aabb();
    return resource_handle_t(mesh, count);
}

resource_handle_t<static_mesh_t> asset_loader_t::create_static_mesh(const std::string& path, utl::vector<static_vertex_t>&& vertices) {
    if (static_mesh_cache.count(path)) { 
        auto& [mesh, count] = static_mesh_cache[path];
        return resource_handle_t(mesh, ++count);
    }
    static_mesh_cache[path] = create_cache_resource<static_mesh_t>();
    new (static_mesh_cache[path].resource) static_mesh_t(std::move(vertices));
    auto& [mesh, count] = static_mesh_cache[path];
    mesh->update_aabb();
    return resource_handle_t(mesh, count);
}

resource_handle_t<static_mesh_t> asset_loader_t::get_static_mesh(const std::string& path) {
    if (static_mesh_cache.count(path)) { 
        auto& [mesh, count] = static_mesh_cache[path];
        return resource_handle_t(mesh, ++count);
    }
    
    throw not_implemented_x();
    return resource_handle_t<static_mesh_t>();
}

resource_handle_t<texture2d_t> asset_loader_t::create_texture2d(const std::string& name, u32 w, u32 h){
    if (texture2d_cache.count(name)) { 
        auto& [texture, count] = texture2d_cache[name];
        return resource_handle_t(texture, ++count);
    }
    texture2d_cache[name] = create_cache_resource<texture2d_t>();
    new (texture2d_cache[name].resource) texture2d_t(w, h);
    auto& [texture, count] = texture2d_cache[name];
    return resource_handle_t(texture, count);
}

resource_handle_t<texture2d_t> asset_loader_t::get_texture2d(const std::string& path){
    if (texture2d_cache.count(path)) { 
        auto& [texture, count] = texture2d_cache[path];
        return resource_handle_t(texture, ++count);
    }
    texture2d_cache[path] = create_cache_resource<texture2d_t>();
    new (texture2d_cache[path].resource) texture2d_t(path, asset_dir);
    auto& [texture, count] = texture2d_cache[path];
    return resource_handle_t(texture, count);
}
resource_handle_t<shader_t> asset_loader_t::get_shader_nameless(const std::string& vs, const std::string& fs){
    return get_shader(fmt::format("{}/{}",vs,fs), {vs, fs});
}
resource_handle_t<shader_t> asset_loader_t::get_shader_vs_fs(const std::string& name, const std::string& vs, const std::string& fs){
    return get_shader(name, {vs, fs});
}
resource_handle_t<shader_t> asset_loader_t::get_shader(const std::string& name, const utl::vector<std::string>& path){
    if (shader_cache.count(name)) { 
        auto& [shader, count] = shader_cache[name];
        return resource_handle_t(shader, ++count);
    }
    shader_cache[name] = create_cache_resource<shader_t>();
    new (shader_cache[name].resource) shader_t(name, path, asset_dir);
    auto& [shader, count] = shader_cache[name];
    return resource_handle_t(shader, count);
}
resource_handle_t<framebuffer_t> asset_loader_t::get_framebuffer(const std::string& name, int w, int h, bool msaa){
    if (framebuffer_cache.count(name)) { 
        auto& [framebuffer, count] = framebuffer_cache[name];
        return resource_handle_t(framebuffer, ++count);
    }
    framebuffer_cache[name] = create_cache_resource<framebuffer_t>();
    new (framebuffer_cache[name].resource) framebuffer_t(w, h, msaa);
    auto& [framebuffer, count] = framebuffer_cache[name];
    return resource_handle_t(framebuffer, count);
}

resource_handle_t<skeletal_model_t> asset_loader_t::get_skeletal_model(const std::string& path) {
    const auto& name = path;
    if (skeletal_model_cache.count(name)) { 
        auto& [model, count] = skeletal_model_cache[name];
        return resource_handle_t(model, ++count);
    }
    skeletal_model_cache[name] = create_cache_resource<skeletal_model_t>();
    new (skeletal_model_cache[name].resource) skeletal_model_t(name, asset_dir);
    auto& [model, count] = skeletal_model_cache[name];
    return resource_handle_t(model, count);
}

resource_handle_t<animation_t> asset_loader_t::get_animation(
    const std::string& path, 
    const std::string& skeleton
) {
    if (animation_cache.count(path)) { 
        auto& [animation, count] = animation_cache[path];
        return resource_handle_t(animation, ++count);
    }
    auto skeleton_handle = get_skeletal_model(skeleton);
    animation_cache[path] = create_cache_resource<animation_t>();
    new (animation_cache[path].resource) animation_t(path, asset_dir, skeleton_handle.get());
    auto& [animation, count] = animation_cache[path];

    return resource_handle_t(animation, count);
}

