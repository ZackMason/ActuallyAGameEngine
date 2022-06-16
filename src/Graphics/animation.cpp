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

animation_t::animation_t(const std::string& animationPath, const std::string& asset_dir, skeletal_model_t& model)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(fmt::format("{}{}", asset_dir, animationPath), aiProcess_Triangulate);
    assert(scene && scene->mRootNode);
    auto animation = scene->mAnimations[0];
    duration = (float)animation->mDuration;
    tps = (int)animation->mTicksPerSecond;
    aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
    globalTransformation = globalTransformation.Inverse();
    read_heirarchy_data(root, scene->mRootNode);
    read_missing_bones(animation, model);
}

void animation_t::read_missing_bones(const aiAnimation* animation, skeletal_model_t& model)
{
    int size = animation->mNumChannels;

    auto& boneInfoMap = model.bone_info;//getting m_BoneInfoMap from Model class
    int& boneCount = model.bone_count; //getting the m_BoneCounter from Model class

    //reading channels(bones engaged in an animation and their keyframes)
    for (int i = 0; i < size; i++)
    {
        auto channel = animation->mChannels[i];
        std::string boneName = channel->mNodeName.data;

        if (boneInfoMap.find(boneName) == boneInfoMap.end())
        {
            boneInfoMap[boneName].id = boneCount;
            boneCount++;
        }
        bones.push_back(bone_t(channel->mNodeName.data,
            boneInfoMap[channel->mNodeName.data].id, channel));
    }

    bone_info = boneInfoMap;
}

void animation_t::read_heirarchy_data(assimp_node_data_t& dest, const aiNode* src)
{
    assert(src);

    dest.name = src->mName.data;
    dest.transform = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);

    for (size_t i = 0; i < src->mNumChildren; i++)
    {
        assimp_node_data_t newData;
        read_heirarchy_data(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}