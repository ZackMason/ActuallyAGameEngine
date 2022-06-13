// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#pragma once

#include "Engine/camera.hpp"
#include "Engine/window.hpp"

struct fly_camera_t : public camera_t {
    using camera_t::camera_t;

    f32 yaw{0.f}; f32 pitch{0.f};
    f32 lastx{0.0f};
    f32 lasty{0.0f};
    
    void update(const window_t& window, f32 dt);

};

