// Author: Zackery Mason-Blaug
// Date: 2022-07-18
//////////////////////////////////////////////////////////


#pragma once

#include "Util/string.hpp"
#include "types.hpp"

#include <fmt/core.h>
#include <array>

using bone_id_t = int;
struct skeleton_bone_t {
    string_id_t name{"invalid"_sid};
    bone_id_t parent{-1};
    m44 offset{1.0f};
};

struct skeleton_t {
    skeleton_t(const std::string& path, const std::string& dir);
    void load(const std::string& path, const std::string& dir);

    [[nodiscard]] bool operator==(const skeleton_t& o) {
        if (count != o.count) return false;
        for (int i{0}; i < count; i++) {
            if (bones[i].name != o.bones[i].name) {
                return false;
            }
        }
        return true;
    }

    template<typename LogFn>
    void log(LogFn fn) {
        for (int i{0}; i < count; i++) {
            fn(fmt::format("[{}] {}: parent: {}", i, sid_to_string(bones[i].name), bones[i].parent ));
        }
    }

    [[nodiscard]] bone_id_t find_bone_id(string_id_t bone_name) const;
    [[nodiscard]] const skeleton_bone_t& find_bone(string_id_t bone_name) const;

    constexpr static size_t max_bones = 256; 

    int count{0};
    std::array<skeleton_bone_t, max_bones> bones;
};

