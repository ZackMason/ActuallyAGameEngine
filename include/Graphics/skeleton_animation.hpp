// Author: Zackery Mason-Blaug
// Date: 2022-07-18
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include "Graphics/bone.hpp"
#include "Graphics/skeleton.hpp"

#include "Util/vector.hpp"

#include <optional>

struct skeleton_animation_t {
    struct node_t {
        std::optional<bone_t> bone;
        bone_id_t parent{-1};
        m44 transform{1.0f};
        m44 offset{1.0f};
    };

    struct loader_t {
        const skeleton_t& skeleton;
        explicit loader_t(const skeleton_t& skeleton);
        [[nodiscard]] utl::vector<skeleton_animation_t> load(const std::string& path, const std::string& dir);
    };

    std::string name;
    float duration{1.0f};
    int ticks_per_sec{24};
    size_t node_count{0};
    std::array<node_t, skeleton_t::max_bones> nodes;
    
};

struct skeleton_animator_t {
    utl::vector<m44> matrices;
    float time{0.0f};
    skeleton_animation_t* animation{nullptr};    

    explicit skeleton_animator_t(skeleton_animation_t* p_anim) :
        animation(p_anim), matrices(200, m44(1.0f))
    {
    }

    void update(float dt){ 
        if (animation) {
            time += dt * animation->ticks_per_sec;
            time = fmod(time, animation->duration - 0.001f);

            for (auto& node : animation->nodes) {
                const auto& bone_transform = node.bone ? node.bone->update(time) : node.transform;
                const auto& parent_transform = (node.parent >= 0) ?  animation->nodes[node.parent].transform : m44(1.0f);
                node.transform = parent_transform * bone_transform;
                if(node.bone) {
                    const auto index = node.bone->id;
                    assert(index < matrices.size() && "Too many bones");
                    matrices[index] = node.transform * node.offset;
                }
            }
        }
    }

    void play_animation(skeleton_animation_t* pAnimation)
	{
		animation = pAnimation;
        std::fill(matrices.begin(), matrices.end(), m44(1.0f));
		time = 0.0f;
	}
};
