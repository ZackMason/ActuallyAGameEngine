// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <type_traits>

template <typename T = v3f>
struct aabb_t {
    T min;
    T max;

    void expand(const aabb_t<T>& o) {
        expand(o.min);
        expand(o.max);
    }

    void expand(const T& p) {
        min = glm::min(p, min);
        max = glm::max(p, max);
    }

    bool contains(const v3f& p) const {
        static_assert(std::is_same<T, v3f>::value);
    
        return 
            min.x <= p.x &&
            min.y <= p.y &&
            min.z <= p.z &&
            p.x <= max.x &&
            p.y <= max.y &&
            p.z <= max.z;
    }
    bool contains(const f32 p) const {
        return min <= p && p <= max;
    }
};

