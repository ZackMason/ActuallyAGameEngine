// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#include "Graphics/bone.hpp"

#include <glm/gtx/quaternion.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include<assimp/quaternion.h>
#include<assimp/vector3.h>
#include<assimp/matrix4x4.h>
#include<glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>


bone_t::bone_t(const std::string& pname, bone_id_t pID, const aiNodeAnim* channel)
    : name(pname),
    id(pID),
    transform(1.0f)
{
    for (size_t positionIndex = 0; positionIndex < channel->mNumPositionKeys; ++positionIndex)
    {
        aiVector3D pos = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = (float)channel->mPositionKeys[positionIndex].mTime;
        position_keyframe_t data;
        data.position = v3f(pos.x, pos.y, pos.z);
        data.timestamp = timeStamp;
        positions.push_back(data);
    }

    for (size_t rotationIndex = 0; rotationIndex < channel->mNumRotationKeys; ++rotationIndex)
    {
        aiQuaternion orientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = (float)channel->mRotationKeys[rotationIndex].mTime;
        rotation_keyframe_t data;
        data.orientation = glm::quat(orientation.w, orientation.x, orientation.y, orientation.z);
        data.timestamp = timeStamp;
        rotations.push_back(data);
    }

    for (size_t keyIndex = 0; keyIndex < channel->mNumScalingKeys; ++keyIndex)
    {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = (float)channel->mScalingKeys[keyIndex].mTime;
        scale_keyframe_t data;
        data.scale = v3f(scale.x, scale.y, scale.z);
        data.timestamp = timeStamp;
        scales.push_back(data);
    }
}

const m44& bone_t::update(float animationTime) {
    glm::mat4 translation = interpolate_position(animationTime);
    glm::mat4 rotation = interpolate_rotation(animationTime);
    glm::mat4 scale = interpolate_scale(animationTime);
    return transform = translation * rotation * scale;
}

int bone_t::get_position_index(float animationTime) {
    for (int index = 0; index < positions.size() - 1; ++index)
    {
        if (animationTime < positions[index + 1].timestamp)
            return index;
    }
    assert(0);
    return -1;
}

int bone_t::get_rotation_index(float animationTime) {
    for (int index = 0; index < rotations.size() - 1; ++index)
    {
        if (animationTime < rotations[index + 1].timestamp)
            return index;
    }
    assert(0);
    return -1;
}

int bone_t::get_scale_index(float animationTime)
{
    for (int index = 0; index < scales.size() - 1; ++index)
    {
        if (animationTime < scales[index + 1].timestamp)
            return index;
    }
    assert(0);
    return -1;
}

float bone_t::get_scale_factor(float lastTimeStamp, float nextTimeStamp, float animationTime)
{
    float scaleFactor = 0.0f;
    float midWayLength = animationTime - lastTimeStamp;
    float framesDiff = nextTimeStamp - lastTimeStamp;
    scaleFactor = midWayLength / framesDiff;
    return scaleFactor;
}

glm::mat4 bone_t::interpolate_position(float anim) {
    return glm::translate(glm::mat4(1.0f), get_position(anim));
}

glm::mat4 bone_t::interpolate_rotation(float anim) {
    return glm::toMat4(get_rotation(anim));
}

glm::mat4 bone_t::interpolate_scale(float anim) {
    return glm::scale(glm::mat4(1.0f), get_scale(anim));
}

v3f bone_t::get_position(float animationTime) {
    if (1 == positions.size()) {
        return positions[0].position;
    }

    int p0 = get_position_index(animationTime);
    int p1 = p0 + 1;
    float scaleFactor = get_scale_factor(positions[p0].timestamp,
        positions[p1].timestamp, animationTime);
    return glm::mix(positions[p0].position, positions[p1].position, scaleFactor);
}

glm::quat bone_t::get_rotation(float animationTime) {
    if (1 == rotations.size()) {
        return glm::normalize(rotations[0].orientation);
    }

    int p0 = get_rotation_index(animationTime);
    int p1 = p0 + 1;
    float scaleFactor = get_scale_factor(rotations[p0].timestamp,
        rotations[p1].timestamp, animationTime);
    glm::quat finalRotation = glm::slerp(rotations[p0].orientation,
        rotations[p1].orientation, scaleFactor);
    
    return glm::normalize(finalRotation);     
}

v3f bone_t::get_scale(float animationTime) {
    if (1 == scales.size()) {
        return scales[0].scale;
    }

    int p0 = get_scale_index(animationTime);
    int p1 = p0 + 1;
    float scaleFactor = get_scale_factor(scales[p0].timestamp, scales[p1].timestamp, animationTime);
    
    return glm::mix(scales[p0].scale, scales[p1].scale, scaleFactor);
}