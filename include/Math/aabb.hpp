// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include <type_traits>
#include <limits>

template <typename T = v3f>
struct aabb_t {
    T min{std::numeric_limits<float>::max()};
    T max{-std::numeric_limits<float>::max()};

    T size() const {
        return max - min;
    }

    T center() const {
        return (max+min)/2.0f;
    }

    void expand(const aabb_t<T>& o) {
        expand(o.min);
        expand(o.max);
    }

    void expand(const T& p) {
        min = glm::min(p, min);
        max = glm::max(p, max);
    }

    bool contains(const v3f& p) const {
        //static_assert(std::is_same<T, v3f>::value);
    
        return 
            min.x <= p.x &&
            min.y <= p.y &&
            min.z <= p.z &&
            p.x <= max.x &&
            p.y <= max.y &&
            p.z <= max.z;
    }

    bool contains(const f32 p) const {
        static_assert(std::is_same<T, f32>::value);
        return min <= p && p <= max;
    }
};

