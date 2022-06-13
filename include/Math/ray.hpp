// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

template <typename T>
struct hit_result_t {
    bool intersect{false};
    f32 distance{0.0f};
    v3f position{0.f, 0.f, 0.f};
    T& object;

    operator bool() const {
        return intersect;
    }
};

struct ray_t {
    v3f origin;
    v3f direction;
};

