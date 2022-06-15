// Author: Zackery Mason-Blaug
// Date: 2022-06-13
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/static_mesh.hpp"
#include "Graphics/texture2d.hpp"

#include "Util/vector.hpp"

struct asset_loader_t;

struct heightmap_t {
    static std::pair<utl::vector<static_vertex_t>, utl::vector<f32>> load_vertices(
        asset_loader_t* loader, 
        const std::string& heightmap_path, 
        f32 max_height, f32 p_w = 1.0f, f32 p_h = 1.0f);
};

