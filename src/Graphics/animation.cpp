// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#include "Graphics/animation.hpp"

#include "assimp/Importer.hpp"
#include "assimp/Scene.h"
#include "assimp/Postprocess.h"

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

#include <stack>

animation_t::animation_t(const std::string& animationPath, const std::string& asset_dir, skeletal_model_t& model)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", asset_dir, animationPath), aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    duration = (float)animation->mDuration;
    tps = (int)animation->mTicksPerSecond;

    auto node_names = read_heirarchy_data(scene->mRootNode, model);
    read_animations(animation, model, node_names);
}

void animation_t::read_animations(const aiAnimation* animation, skeletal_model_t& model, const std::vector<const aiString*>& node_names)
{
    int size = animation->mNumChannels;

    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        const auto& bone_name = channel->mNodeName;
        auto it = std::find_if(node_names.cbegin(), node_names.cend(),
            [&bone_name](const aiString* node_name)
            {
                return (bone_name == *node_name);
            });
        if (it == node_names.end()) continue;
        assert(it != node_names.end() && "No node matching a bone.");
        const bone_id_t bone_index = bone_id_t(std::distance(node_names.cbegin(), it));

        if (model.bone_info.count(bone_name.C_Str()) == 0) continue;
        assert(model.bone_info.count(bone_name.C_Str()) > 0 && "No bone info remap matching a bone.");
        const bone_info_t& bone_info = model.bone_info[bone_name.C_Str()];
        anim_nodes[bone_index].offset = bone_info.offset;

        assert(bone_index < anim_nodes.size() && "Invalid bone index");
        anim_nodes[bone_index].bone.emplace(channel->mNodeName.data, model.bone_info[channel->mNodeName.data].id, channel);
    }
}

std::vector<const aiString*> animation_t::read_heirarchy_data(const aiNode* src, skeletal_model_t& model)
{
    assert(src);

    struct node_id_t {
        const aiNode* s{nullptr};
        bone_id_t parent;
    };
    std::vector<const aiString*> node_names;
    std::stack<node_id_t> stack;
    stack.push({src, -1});

    while (stack.size() > 0) { 
        node_id_t n = std::move(stack.top());
        stack.pop();

        anim_node_t node;
        node.parent = n.parent;
        node.transform =  AssimpGLMHelpers::ConvertMatrixToGLMFormat(n.s->mTransformation);
        assert(node.parent < int(anim_nodes.size()) && "Invalid parent");
        bone_id_t parent_index = bone_id_t(anim_nodes.size());
        if (model.bone_info.count(n.s->mName.data) > 0) {
            node_names.push_back(&n.s->mName);
            anim_nodes.push_back(node);
        } else {
            parent_index = -1;
        }
        for (size_t i = 0; i < n.s->mNumChildren; i++)
        {
            stack.push({n.s->mChildren[i], parent_index});
        }
    }
    return node_names;
}