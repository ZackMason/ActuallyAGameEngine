// Author: Zackery Mason-Blaug
// Date: 2022-10-23
//////////////////////////////////////////////////////////


#include "Util/mesh_loaders.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"



#include <stack>

namespace mesh_loaders {

auto make_v3f(const auto& v) -> v3f {
    return {v.x, v.y, v.z};
}

void process_mesh(aiMesh* mesh, const aiScene* scene, load_results_t& results){
    results.meshes.back().mesh_name = save_string(mesh->mName.C_Str());
    
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        static_vertex_t vertex;
        
        vertex.position = make_v3f(mesh->mVertices[i]);

        if (mesh->mNormals) {
        vertex.normal = make_v3f(mesh->mNormals[i]);

        }
        
#if 0
        if (mesh->mTangents)
            vertex.tangent = to_v3f(mesh->mTangents[i]);
        if (mesh->mBitangents)
            vertex.bitangent = to_v3f(mesh->mBitangents[i]);
#endif
        
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coord = vec;
        } else {
            vertex.tex_coord = glm::vec2(0.0f, 0.0f);
        }

        results.meshes.back().vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            results.meshes.back().indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    aiColor3D color;
    material->Get(AI_MATKEY_COLOR_DIFFUSE, color);
    results.meshes.back().material.base_color = { color.r, color.g, color.b };

    material->Get(AI_MATKEY_COLOR_AMBIENT, color);
    results.meshes.back().material.emissive = { color.r };    

    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
        aiString diffuse_name;
        material->GetTexture(aiTextureType_DIFFUSE, 0, &diffuse_name);
        results.meshes.back().material.albedo_texture = save_string(diffuse_name.C_Str());
    }

    aiString name;
    material->Get(AI_MATKEY_NAME, name);
    results.meshes.back().material_name = save_string(name.C_Str());
}


void process_node(aiNode *start, const aiScene *scene, load_results_t& results){

    std::stack<const aiNode*> stack;
    stack.push(scene->mRootNode);
    while (!stack.empty()) {

        const aiNode* node = stack.top();
        stack.pop();

        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            results.meshes.emplace_back();
            process_mesh(mesh, scene, results);
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            stack.push(node->mChildren[i]);
        }
    }
}

load_results_t load_obj(std::string_view path, std::string_view asset_dir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", asset_dir, path), 
        aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_GlobalScale);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw runtime_error_x(importer.GetErrorString());
    }

    load_results_t results;
    process_node(scene->mRootNode, scene, results);
    return results;
}

std::vector<material_mesh_pair_t> get_mesh_names(std::string_view path, std::string_view asset_dir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", asset_dir, path), 
        0);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw runtime_error_x(importer.GetErrorString());
    }

    std::vector<material_mesh_pair_t> mesh_names;
    std::stack<const aiNode*> stack;
    stack.push(scene->mRootNode);
    while (!stack.empty()) {
        const aiNode* node = stack.top();
        stack.pop();

        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
            
            aiString name;
            material->Get(AI_MATKEY_NAME, name);
            
            mesh_names.emplace_back(material_mesh_pair_t{
                save_string(fmt::format("{}::{}", mesh->mName.C_Str(), path)), 
                save_string(name.C_Str())
            });
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            stack.push(node->mChildren[i]);
        }
    }
    return mesh_names;
}

};