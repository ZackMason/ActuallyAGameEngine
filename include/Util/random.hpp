// Author: Zackery Mason-Blaug
// Date: 2022-06-04
//////////////////////////////////////////////////////////


#pragma once
#define WIN32_MEAN_AND_LEAN
#define NOMINMAX

#include <stdlib.h>
#include <time.h>  

#include <cstdint>
#include <limits>
#include <array>

#include "Math/aabb.hpp"

struct xoshiro256_random_t {
    std::array<uint64_t, 4> state;
    static uint64_t max;

    void randomize() {
        if constexpr (sizeof(time_t) >= 8) {
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
        uint64_t *s = state.data();
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
    static uint64_t max;

    void randomize() {
        if constexpr (sizeof(time_t) >= 8) {
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
    static uint32_t max;

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
    }

    void seed(const decltype(Generator::state)& x) {
        rng.state = x;
    }

    auto rand() {
        return rng.rand();
    }

    float randf() {
        return float(rng.rand()) / float(Generator::max);
    }

    float randn() {
        return randf() * 2.0f - 1.0f;
    }

    template <typename T>
    auto choice(const T& indexable) -> const typename T::value_type& {
        return indexable[rng.rand()%indexable.size()];
    }

    v3f aabb(const aabb_t<v3f>& box) {
        return v3f{
            (this->randf() * (box.max.x - box.min.x)) + box.min.x,
            (this->randf() * (box.max.y - box.min.y)) + box.min.y,
            (this->randf() * (box.max.z - box.min.z)) + box.min.z
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
    static uint64_t rand() {
        return state.rand();
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

