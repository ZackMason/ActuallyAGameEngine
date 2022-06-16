// Author: Zackery Mason-Blaug
// Date: 2022-06-15
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include <string>

#include "Util/vector.hpp"
#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>


namespace AssimpGLMHelpers {
	static glm::mat4 ConvertMatrixToGLMFormat(const aiMatrix4x4& from)
	{
		glm::mat4 to;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
		to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
		to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
		to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
		return to;
	}

	glm::vec3 GetGLMVec(const aiVector3D& vec);
	glm::quat GetGLMQuat(const aiQuaternion& pOrientation);
};

struct position_keyframe_t {
    v3f position{};
    float timestamp{};
};

struct rotation_keyframe_t {
    glm::quat orientation{};
    float timestamp{};
};

struct scale_keyframe_t {
    v3f scale{};
    float timestamp{};
};

struct aiNodeAnim;
struct bone_t {
    utl::vector<position_keyframe_t> positions;
    utl::vector<rotation_keyframe_t> rotations;
    utl::vector<scale_keyframe_t>    scales;

    m44 transform;
    std::string name;
    int id;

    void update(float anim);
    int get_position_index(float anim);
    int get_rotation_index(float anim);
    int get_scale_index(float anim);
    float get_scale_factor(float last, float next, float anim);

    glm::mat4 interpolate_position(float anim);
    glm::mat4 interpolate_rotation(float anim);
    glm::mat4 interpolate_scale(float anim);

    explicit bone_t(const std::string& pname, int pID, const aiNodeAnim* channel);
};

struct bone_info_t {
    int id{-1};
    m44 offset{1.0f};
};