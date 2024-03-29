// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Util/vector.hpp"

#include "Math/aabb.hpp"

#include "Graphics/vertex_array.hpp"
#include "Graphics/buffer.hpp"
#include "Graphics/texture2d.hpp"
#include "Graphics/bone.hpp"
#include "Graphics/skeleton.hpp"

#include "Engine/resource_handle.hpp"

struct skinned_vertex_t {
    v3f position;
    v3f normal;
    v2f tex_coord;
    v3f tangent;
    v3f bitangent;
    int id[4];
    v4f weight{0.0f};
};


struct skeletal_mesh_t : drawable_i {
    buffer_t<utl::vector<skinned_vertex_t>> buffer_object;
    vertex_array_t vertex_array;
    utl::vector<resource_handle_t<texture2d_t>> textures;

    aabb_t<v3f> aabb;

    void update_aabb();

    void bind() override;
    void unbind() override;
    void draw() override;

    explicit skeletal_mesh_t(utl::vector<skinned_vertex_t>&& p_vertices) 
        : buffer_object(std::move(p_vertices)), 
        vertex_array(buffer_object.id, buffer_object.size(), sizeof(skinned_vertex_t))
    { 
        vertex_array
            .set_attrib(0, 3, GL_FLOAT, offsetof(skinned_vertex_t, position))
            .set_attrib(1, 3, GL_FLOAT, offsetof(skinned_vertex_t, normal))
            .set_attrib(2, 2, GL_FLOAT, offsetof(skinned_vertex_t, tex_coord))
            .set_attrib(3, 3, GL_FLOAT, offsetof(skinned_vertex_t, tangent))
            .set_attrib(4, 3, GL_FLOAT, offsetof(skinned_vertex_t, bitangent))
            .set_attribi(5, 4, GL_INT, offsetof(skinned_vertex_t, id))
            .set_attrib(6, 4, GL_FLOAT, offsetof(skinned_vertex_t, weight));
    }
};

struct aiNode;
struct aiScene;
struct aiMesh;
struct skeletal_model_t {
    utl::vector<skeletal_mesh_t*> meshes;
    skeleton_t skeleton;

    // very slow, requires file IO
    void set_skeleton(const std::string& path, const std::string& dir) {
        skeleton = skeleton_t(path, dir);
        for (auto mesh: meshes) {
            delete mesh;
        }
        meshes.clear();
        load_model(path, dir);
    }

    virtual ~skeletal_model_t() {
        for(auto mesh: meshes){
        //    delete mesh;
        }
    }
    
    skeletal_model_t(
        const std::string& path, 
        const std::string& asset_dir
    ) : skeleton(path, asset_dir) {
        load_model(path, asset_dir);
    }
    
    void load_model(const std::string& path, const std::string& asset_dir);
    void process_node(aiNode *node, const aiScene *scene);
    void set_bone_default(skinned_vertex_t& vertex);
    skeletal_mesh_t* process_mesh(aiMesh* mesh, const aiScene* scene);
    void extract_bone_weight(std::vector<skinned_vertex_t>& vertices, aiMesh* mesh, const aiScene* scene);
};
