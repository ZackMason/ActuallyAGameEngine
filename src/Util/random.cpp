
#include "Util/random.hpp"

uint64_t xoshiro256_random_t::max = std::numeric_limits<uint64_t>::max();
uint64_t xor64_random_t::max = std::numeric_limits<uint64_t>::max();
uint32_t xor32_random_t::max = std::numeric_limits<uint32_t>::max();