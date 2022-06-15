// Author: Zackery Mason-Blaug
// Date: 2022-06-04
//////////////////////////////////////////////////////////


#pragma once

#include <stdlib.h>
#include <time.h>  

struct random_s {
    static void randomize() {
        srand((unsigned int)time(0));
    }
    //random float
    static float randf() {
        return rand() / float(RAND_MAX);
    }
    // random normal
    static float randn() {
        return randf() * 2.0f - 1.0f;
    }
    template <typename AABB>
    static auto aabb(const AABB& box) -> decltype(box.min) {
        const auto size = box.max - box.min;
        return decltype(box.min){
            randf() * size.x + box.min.x,
            randf() * size.y + box.min.y,
            randf() * size.z + box.min.z
        };
    }
};

