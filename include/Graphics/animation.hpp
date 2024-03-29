// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"
#include <string>
#include <optional>

#include "Util/vector.hpp"

#include "Graphics/bone.hpp"
#include "Graphics/skeletal_mesh.hpp"

struct anim_node_t {
    std::optional<bone_t> bone;
    m44 transform{1.0f};
    m44 offset{1.0f};
    bone_id_t parent{-1};    
};

struct aiAnimation;
struct aiString;
struct animation_t {
    float duration{1.0f};
    int tps{24}; // ticks per second
    utl::vector<anim_node_t> anim_nodes;

    animation_t() = default;
    animation_t(const std::string& animationPath, const std::string& asset_dir, skeletal_model_t& model);
    virtual ~animation_t() = default;

    std::optional<bone_t> find_bone(const std::string& name)
    {
        auto iter = std::find_if(anim_nodes.begin(), anim_nodes.end(),
            [&](const anim_node_t& node) {   
                if (node.bone) {
                    return node.bone->name == name;
                }
                return false;
            }
        );
        if (iter == anim_nodes.end()) return std::nullopt;
        else return iter->bone;
    }

    void read_animations(const aiAnimation* animation, skeletal_model_t& model, const std::vector<const aiString*>& node_names);
    std::vector<const aiString*> read_heirarchy_data(const aiNode* src, skeletal_model_t& model);
};

struct animator_t {
    utl::vector<m44> matrices;
    float time{0.0f};
    animation_t* animation{nullptr};    

    explicit animator_t(animation_t* p_anim) :
        animation(p_anim), matrices(100, m44(1.0f))
    {
    }

    void update(float dt){ 
        if (animation) {
            time += dt * animation->tps;
            time = fmod(time, animation->duration-1.0f);

            for (auto& node : animation->anim_nodes) {
                const auto& bone_transform = node.bone ? node.bone->update(time) : node.transform;
                const auto& parent_transform = (node.parent >= 0) ?
                    animation->anim_nodes[node.parent].transform : m44(1.0f);
                node.transform = parent_transform * bone_transform;
                if(node.bone) {
                    const auto index = node.bone->id;
                    assert(index < matrices.size() && "Too many bones");
                    matrices[index] =  node.transform * node.offset;
                }
            }
        }
    }

    void play_animation(animation_t* pAnimation)
	{
		animation = pAnimation;
        std::fill(matrices.begin(), matrices.end(), m44(1.0f));
		time = 0.0f;
	}
};

