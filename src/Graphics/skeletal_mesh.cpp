// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#include "Graphics/skeletal_mesh.hpp"

#include "Util/exceptions.hpp"
#include "Util/logger.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include <stack>

void skeletal_mesh_t::update_aabb() {
    for (const auto& vertex: buffer_object.data) {
        aabb.expand(vertex.position);
    }
}
void skeletal_mesh_t::bind() {
    vertex_array.bind();
}

void skeletal_mesh_t::unbind() {
    vertex_array.unbind();
}

void skeletal_mesh_t::draw() {
    vertex_array.draw();
}

void skeletal_model_t::load_model(const std::string& path, const std::string& asset_dir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", asset_dir, path), 
    aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_CalcTangentSpace | aiProcess_GlobalScale);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        throw runtime_error_x(importer.GetErrorString());
    }

    process_node(scene->mRootNode, scene);
}

void skeletal_model_t::process_node(aiNode *start, const aiScene *scene){

    std::stack<const aiNode*> stack;
    stack.push(scene->mRootNode);
    while (!stack.empty()) {

        const aiNode* node = stack.top();
        stack.pop();

        for(unsigned int i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            meshes.push_back(process_mesh(mesh, scene));
        }
        for(unsigned int i = 0; i < node->mNumChildren; i++) {
            stack.push(node->mChildren[i]);
        }
    }
}
void skeletal_model_t::set_bone_default(skinned_vertex_t& vertex){
    for (int i = 0; i < 4; i++) {
        vertex.id[i] = -1;
        vertex.weight[i] = 0.0f;
    }
}
auto to_v3f(const auto& v) -> v3f {
    return {v.x, v.y, v.z};
}

skeletal_mesh_t* skeletal_model_t::process_mesh(aiMesh* mesh, const aiScene* scene){
    utl::vector<skinned_vertex_t> indexed_vertices;
    utl::vector<skinned_vertex_t> vertices;
    utl::vector<unsigned int> indices;
    //utl::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        skinned_vertex_t vertex;
        set_bone_default(vertex);
        vertex.position = to_v3f(mesh->mVertices[i]);
        vertex.normal = to_v3f(mesh->mNormals[i]);
        if (mesh->mTangents)
            vertex.tangent = to_v3f(mesh->mTangents[i]);
        if (mesh->mBitangents)
            vertex.bitangent = to_v3f(mesh->mBitangents[i]);
        
        if (mesh->mTextureCoords[0]) {
            glm::vec2 vec;
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coord = vec;
        } else {
            vertex.tex_coord = glm::vec2(0.0f, 0.0f);
        }

        indexed_vertices.push_back(vertex);
    }
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    //vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
    //textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    //vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
    //textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    //std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal");
    //textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
    //std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height");
    //textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

    extract_bone_weight(indexed_vertices,mesh,scene);

    std::transform(
        indices.begin(), 
        indices.end(), 
        std::back_inserter(vertices),
        [&](auto i){ return indexed_vertices[i]; });

    return new skeletal_mesh_t(std::move(vertices));
}
void set_vertex_bone_data(skinned_vertex_t& vertex, int bone_id, float weight) {
    for (int i = 0; i < 4; ++i) {
        if (vertex.id[i] < 0) {
            vertex.weight[i] = weight;
            vertex.id[i] = bone_id;
            break;
        }
    }
}

void skeletal_model_t::extract_bone_weight(std::vector<skinned_vertex_t>& vertices, aiMesh* mesh, const aiScene* scene) {
    for (size_t boneIndex = 0; boneIndex < mesh->mNumBones; ++boneIndex) {
        const std::string bone_name = mesh->mBones[boneIndex]->mName.C_Str();
        const int bone_id = skeleton.find_bone_id(sid(bone_name));
        
        if (bone_id == -1) {
            logger_t::warn(fmt::format("Skeleton is missing bone: {}", bone_name));
        }
        assert(bone_id != -1 && "Skeleton is missing bones");

        auto weights = mesh->mBones[boneIndex]->mWeights;
        int numWeights = mesh->mBones[boneIndex]->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            assert(vertexId < vertices.size());
            set_vertex_bone_data(vertices[vertexId], bone_id, weight);
        }
    }
}