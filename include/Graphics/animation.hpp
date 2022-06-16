// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"
#include <string>

#include "Util/vector.hpp"

#include "Graphics/bone.hpp"
#include "Graphics/skeletal_mesh.hpp"

struct assimp_node_data_t {
    m44 transform{1.0f};
    std::string name{};
    utl::vector<assimp_node_data_t> children;
};

struct aiAnimation;
struct animation_t {


    float duration{1.0f};
    int tps{24}; // ticks per second
    std::vector<bone_t> bones;
    assimp_node_data_t root;
    std::unordered_map<std::string, bone_info_t> bone_info;

    animation_t() = default;
    animation_t(const std::string& animationPath, const std::string& asset_dir, skeletal_model_t& model);
    virtual ~animation_t() = default;

    bone_t* find_bone(const std::string& name)
    {
        auto iter = std::find_if(bones.begin(), bones.end(),
            [&](const bone_t& bone)
            {
                return bone.name == name;
            }
        );
        if (iter == bones.end()) return nullptr;
        else return &(*iter);
    }

    void read_missing_bones(const aiAnimation* animation, skeletal_model_t& model);
    void read_heirarchy_data(assimp_node_data_t& dest, const aiNode* src);
};

struct animator_t {
    explicit animator_t(animation_t* p_anim) :
        animation(p_anim), matrices(100, m44(1.0f))
    {
    }

    void update(float dt){ 
        if (animation) {
            time += dt * animation->tps;
            time = fmod(time, animation->duration);
            calculate_bone_transform(&animation->root, m44(1.0f));
        }
    }

    void play_animation(animation_t* pAnimation)
	{
		animation = pAnimation;
		time = 0.0f;
	}

    void calculate_bone_transform(const assimp_node_data_t* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4 nodeTransform = node->transform;

		bone_t* bone = animation->find_bone(nodeName);

		if (bone)
		{
			bone->update(time);
			nodeTransform = bone->transform;
		}

		glm::mat4 globalTransformation = parentTransform * nodeTransform;

		auto& boneInfoMap = animation->bone_info;
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			matrices[index] = globalTransformation * offset;
		}

		for (int i = 0; i < node->children.size(); i++)
			calculate_bone_transform(&node->children[i], globalTransformation);
	}

    utl::vector<m44> matrices;
    float time{0.0f};
    animation_t* animation{nullptr};    
};
