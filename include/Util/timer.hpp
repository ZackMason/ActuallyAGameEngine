// Author: Zackery Mason-Blaug
// Date: 2022-06-11
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

struct timer32_t {

    f32 get_dt(f32 ticks) {
        dt = ticks - last_time;
        last_time = ticks;
        return dt;
    }

    f32 dt{0.0f};
    f32 last_time{0.0f};
};

