// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#pragma once

#include "Engine/orbit_camera.hpp"
#include "Engine/fly_camera.hpp"

struct orbit_fly_camera_t {
    orbit_fly_camera_t(f32 f, f32 w, f32 h, f32 n, f32 fa)
    : fly_camera(f,w,h,n,fa), orbit_camera(f,w,h,n,fa)
    {
        fly_camera.make_projection();
        orbit_camera.make_projection();
    }


    fly_camera_t fly_camera;
    orbit_camera_t orbit_camera;

    bool is_fly{true};

    auto view_projection() const {
        return is_fly ? fly_camera.view_projection() : orbit_camera.view_projection();
    }
    void update(window_t& window, f32 dt);
};

