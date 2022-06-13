#pragma once

#include <cstdint>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

using f32 = float;
using f64 = double;

#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"

using v2f = glm::vec2;
using v3f = glm::vec3;
using v4f = glm::vec4;

using v2i = glm::vec<2, i32>;
using v3i = glm::vec<3, i32>;
using v4i = glm::vec<4, i32>;

using v2u = glm::vec<2, u32>;
using v3u = glm::vec<3, u32>;
using v4u = glm::vec<4, u32>;

using quat = glm::quat;
using m22 = glm::mat2x2;
using m33 = glm::mat3;
using m34 = glm::mat3x4;
using m43 = glm::mat4x3;
using m44 = glm::mat4x4;
