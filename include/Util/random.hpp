// Author: Zackery Mason-Blaug
// Date: 2022-06-04
//////////////////////////////////////////////////////////


#pragma once

#include <stdlib.h>
#include <time.h>  

#include <cstdint>

struct xoshiro256_random_t {
    uint64_t state[4];
    inline static uint64_t max = ~0;

    void randomize() {
        if (sizeof(time_t) >= 8) {
            state[3] = static_cast<uint64_t>(time(0));
            state[2] = static_cast<uint64_t>(time(0));
            state[1] = static_cast<uint64_t>(time(0));
            state[0] = static_cast<uint64_t>(time(0));
        }
        else {
            state[3] = static_cast<uint64_t>(time(0) | (~time(0) << 32));
            state[2] = static_cast<uint64_t>(time(0) | (~time(0) << 32));
            state[1] = static_cast<uint64_t>(time(0) | (~time(0) << 32));
            state[0] = static_cast<uint64_t>(time(0) | (~time(0) << 32));
        }
    };

    uint64_t rand() {
        uint64_t *s = state;
        uint64_t const result = rol64(s[1] * 5, 7) * 9;
        uint64_t const t = s[1] << 17;

        s[2] ^= s[0];
        s[3] ^= s[1];
        s[1] ^= s[2];
        s[0] ^= s[3];

        s[2] ^= t;
        s[3] = rol64(s[3], 45);

        return result;
    }

    uint64_t rol64(uint64_t x, int k) {
        return (x << k) | (x >> (64 - k));
    }
};

struct xor64_random_t {
    uint64_t state;
    inline static uint64_t max = ~0;

    void randomize() {
        if (sizeof(time_t) >= 8) {
            state = static_cast<uint64_t>(time(0));
        }
        else {
            state = static_cast<uint64_t>(time(0) | (~time(0) << 32));
        }
    };

    uint64_t rand(){
        auto x = state;
        x ^= x << 13;
        x ^= x >> 7;
        x ^= x << 17;
        return state = x;
    }
};

struct xor32_random_t {
    uint32_t state;
    inline static uint32_t max = ~0;

    void randomize() {
        state = static_cast<uint32_t>(time(0));
    };

    uint32_t rand(){
        auto x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        return state = x;
    }
};

template <typename Generator>
struct random_t {
    Generator rng;

    void randomize() {
        rng.randomize();
    };

    float randf() {
        return float(rng.rand()) / float(Generator::max);
    }

    float randn() {
        return randf() * 2.0f - 1.0f;
    }

    template <typename AABB>
    auto aabb(const AABB& box) -> decltype(box.min) {
        const auto size = box.max - box.min;
        return decltype(box.min){
            randf() * size.x + box.min.x,
            randf() * size.y + box.min.y,
            randf() * size.z + box.min.z
        };
    }
};

struct random_s {
    inline static random_t<xor64_random_t> state;

    static void randomize() {
        state.randomize();
    }
    //random float
    static float randf() {
        return state.randf();
    }
    // random normal
    static float randn() {
        return state.randn();
    }
    template <typename AABB>
    static auto aabb(const AABB& box) -> decltype(box.min) {
        return state.aabb(box);
    }
};

