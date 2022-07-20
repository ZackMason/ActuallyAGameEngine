// Author: Zackery Mason-Blaug
// Date: 2022-07-18
//////////////////////////////////////////////////////////


#include "Graphics/skeleton.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Graphics/bone.hpp"

#include "Util/logger.hpp"

#include <stack>

skeleton_t::skeleton_t(const std::string& path, const std::string& dir) {
    load(path, dir);
}

struct bone_pair_t {
    std::string name;
    m44 offset;
};

void read_heirarchy_data(skeleton_t& skeleton, const aiNode* root, const utl::vector<bone_pair_t>& bone_names) {
    struct parse_node_t {
        const aiNode* node{nullptr};
        bone_id_t parent{-1};
    };
    std::stack<parse_node_t> stack;
    stack.push({root, -1});

    while (!stack.empty()) {
        parse_node_t node = std::move(stack.top());
        stack.pop();

        assert(node.node);    

        bone_id_t bone_id = -1;
        //if (std::find(bone_names.cbegin(), bone_names.cend(), node.node->mName.C_Str()) != bone_names.cend()) {

            skeleton_bone_t bone;
            bone.name = save_string(node.node->mName.C_Str());
            bone.parent = node.parent;
            for (auto& [bone_name, offset]: bone_names) {
                if (bone_name == node.node->mName.C_Str()) {
                    bone.offset = offset;
                }
            }
    
            assert(skeleton.count < skeleton_t::max_bones);
            bone_id = skeleton.count++;
            skeleton.bones[bone_id] = bone;
        //}

        for (size_t i = 0; i < node.node->mNumChildren; i++) {
            stack.push({node.node->mChildren[i], bone_id});
        }
    }
}

[[nodiscard]] utl::vector<bone_pair_t> parse_bone_names(const aiScene* scene) {
    assert(scene);

    utl::vector<bone_pair_t> bone_names;

    for (size_t i{ 0 }; i < scene->mNumMeshes; i++) {
        const auto mesh = scene->mMeshes[i];
        if (mesh->HasBones()) {
            for (size_t j{ 0 }; j < mesh->mNumBones; j++) {
                std::string bone_name = mesh->mBones[j]->mName.C_Str();
                if (std::find_if(bone_names.begin(), bone_names.end(), [&](auto& a){ return a.name == bone_name; }) == bone_names.end()) {
                    bone_names.push_back({
                        bone_name, 
                        AssimpGLMHelpers::ConvertMatrixToGLMFormat(mesh->mBones[j]->mOffsetMatrix)
                    });
                }
            }
        }
    }
    logger_t::info(fmt::format("Found {} bones", bone_names.size()));
    return bone_names;
}

void skeleton_t::load(const std::string& path, const std::string& dir) {
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", dir, path), aiProcess_Triangulate | aiProcess_GlobalScale);
    
    assert(scene && scene->mRootNode);
    
    read_heirarchy_data(*this, scene->mRootNode, parse_bone_names(scene));
    for (auto& bone: bones) {
    }
}

[[nodiscard]] bone_id_t skeleton_t::find_bone_id(string_id_t bone_name) const {
    for (bone_id_t i = 0; const auto& bone: bones) {
        if (bone.name == bone_name) {
            return i;
        }
        i++;
    }
    return -1;
}

[[nodiscard]] const skeleton_bone_t& skeleton_t::find_bone(string_id_t bone_name) const {
    const auto bone_id = find_bone_id(bone_name);
    static skeleton_bone_t dummy_bone{};
    return bone_id != -1 ? bones[bone_id] : dummy_bone; 
}
