#pragma once

#include <memory>
#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <string>
#include <fstream>
#include <cassert>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <functional>

#include "types.hpp"

#include <fmt/core.h>
#include <fmt/color.h>


#ifndef CMAKE_ASSETS_PATH
#define CMAKE_ASSETS_PATH "./"
#endif

#define ASSETS_PATH std::string(CMAKE_ASSETS_PATH)

constexpr int BIT(int x)
{
	return 1 << x;
}
#pragma warning( disable : 4005 )


template <typename T>
inline T lerp(T a, T b, f32 t)
{
	return b * t + a * (1.0f - t);
}
template <typename T>
inline T lerp_dt(T a, T b, f32 s, f32 dt)
{
	return lerp(b, a, std::pow(1.0f - s, dt));
}
template <typename T>
inline T damp(T a, T b, f32 lambda, f32 dt)
{
	return lerp(a, b, 1.0f - std::exp(-lambda * dt));
}


template <typename T>
inline void clamp_length(T& v, f32 max_length) {
	if (glm::dot(v, v) > max_length * max_length) {
		v = glm::normalize(v) * max_length;
	}
}
