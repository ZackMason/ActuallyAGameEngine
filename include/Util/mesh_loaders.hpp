// Author: Zackery Mason-Blaug
// Date: 2022-10-23
//////////////////////////////////////////////////////////


#pragma once

#include "Util/string.hpp"
#include "Util/allocators.hpp"


#include "Graphics/material.hpp"
#include "Graphics/static_mesh.hpp"

#include <vector>

namespace mesh_loaders {
    struct load_results_t {
        struct mesh_t {
            std::vector<static_vertex_t> vertices;
            std::vector<unsigned int> indices;
            material_t material;        
            sid_t mesh_name;
            sid_t material_name;
        };
        std::vector<mesh_t> meshes;
    };

    load_results_t load_obj(std::string_view path, std::string_view asset_dir);
    
    struct material_mesh_pair_t {
        sid_t mesh;
        sid_t material;
    };
    std::vector<material_mesh_pair_t> get_mesh_names(std::string_view path, std::string_view asset_dir);
};

