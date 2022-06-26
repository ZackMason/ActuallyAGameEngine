// Author: Zackery Mason-Blaug
// Date: 2022-06-13
//////////////////////////////////////////////////////////


#pragma once

#include <entt/entt.hpp>

#include "Graphics/static_mesh.hpp"
#include "Graphics/texture2d.hpp"

#include "Util/vector.hpp"

struct asset_loader_t;

struct heightmap_t {

    entt::entity entity = entt::null;
    u32 width{0}, height{0}; // number of vertices
    f32 tile_width{1.0f}, tile_height{1.0f}; // distance between vertices
    utl::vector<f32> heights;



    auto map_height() const {
        return tile_height * f32(height);
    }

    auto map_width() const {
        return tile_width * f32(width);
    }

    static std::pair<utl::vector<static_vertex_t>, utl::vector<f32>> load_vertices(
        asset_loader_t* loader, 
        const std::string& heightmap_path, 
        f32 max_height, f32 p_w = 1.0f, f32 p_h = 1.0f);
};

