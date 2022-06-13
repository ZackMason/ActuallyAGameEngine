// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

template <typename T = v3f>
struct aabb_t {
    T min;
    T max;

    bool contains(const T& p) const {
        return min <= p && p <= max;
    }
};

