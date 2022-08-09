// Author: Zackery Mason-Blaug
// Date: 2022-07-18
//////////////////////////////////////////////////////////


#include "Graphics/skeleton_animation.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"


#include "Util/logger.hpp"

#include <algorithm>

skeleton_animation_t::loader_t::loader_t(
    const skeleton_t& _skeleton 
) : skeleton(_skeleton) {

}

skeleton_animator_t skeleton_animator_t::blend(skeleton_animator_t& b, f32 perc) const {
    auto& a = *this;
    assert(a.animation);
    assert(b.animation);

    perc = std::clamp(perc, 0.0f, 1.0f);

    b.time = a.time;
    b.update(0.0f);
    skeleton_animator_t res{a.animation};
    
    res.time = a.time;

    static constexpr bool blend_mats = true;

    if constexpr (blend_mats) {
        for (int i = 0; i < a.animation->node_count; i++ ) {
            res.matrices[i] = a.matrices[i] * (1.0f-perc) + b.matrices[i] * perc;
        }
    } else {
        for (auto& node : a.animation->nodes) {

        }
    }
    return res;
}

void load_animation(
    skeleton_animation_t& animation,
    const aiNode* ai_root,
    const aiAnimation* ai_anim, 
    const skeleton_t& skeleton
) {
    assert(ai_anim);
    assert(ai_root);
    const auto size = ai_anim->mNumChannels;
    animation.name = ai_anim->mName.C_Str();

    for(size_t i{0}; i < size; i++) {
        const auto channel = ai_anim->mChannels[i];

        const std::string bone_name = channel->mNodeName.C_Str();
        const auto bone_id = skeleton.find_bone_id(sid(bone_name));
        if (bone_id == -1) {
            logger_t::error(fmt::format("Failed to find bone {}", bone_name));
            continue;
        }
        assert(bone_id != -1);
        const auto& bone = skeleton.find_bone(sid(bone_name));

        animation.node_count = bone_id >= animation.node_count ? bone_id + 1 : animation.node_count;
        animation.nodes[bone_id].offset = bone.offset;
        animation.nodes[bone_id].parent = bone.parent;
        const auto ai_node = ai_root->FindNode(bone_name.c_str());
        if (ai_node) {
            animation.nodes[bone_id].transform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(ai_node->mTransformation);
        }
        else {
            logger_t::warn(fmt::format("Failed to find node for transform: {}", bone_name));
            animation.nodes[bone_id].transform = m44(1.0f);
        }
        animation.nodes[bone_id].bone.emplace(channel->mNodeName.data, bone_id, channel);
    }
}

utl::vector<skeleton_animation_t> skeleton_animation_t::loader_t::load(
    const std::string& path,
    const std::string& dir
) {
    utl::vector<skeleton_animation_t> results;
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", dir, path), aiProcess_Triangulate | aiProcess_GlobalScale);
    
    assert(scene && scene->mRootNode);
    
    logger_t::info(fmt::format("Animation Loader: Loading {} animations", scene->mNumAnimations));
    for (size_t i{ 0 }; i < scene->mNumAnimations; i++) {
        auto ai_animation = scene->mAnimations[i];
        results.push_back(skeleton_animation_t{});
        auto& animation = results.back();
        animation.duration = (float)ai_animation->mDuration;
        animation.ticks_per_sec = (int)ai_animation->mTicksPerSecond;
        load_animation(animation, scene->mRootNode, ai_animation, skeleton);
    }

    return results;
}