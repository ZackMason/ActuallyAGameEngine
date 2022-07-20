// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"
#include "Engine/window.hpp"
#include "Engine/camera.hpp"

struct orbit_camera_t : public camera_t {
    using camera_t::camera_t;

    f32 dist{1.0f};
    f32 yaw{0.f}; f32 pitch{0.f};
    f32 lastx{0.0f};
    f32 lasty{0.0f};

    bool on_scroll_event(const mouse_scroll_event_t& event) {
        dist += -event.y * 5.0f;
        dist = dist < 1.0f ? 1.0f : dist; 
        return true;
    }

    void update(window_t& window, f32 dt);
    v3f get_direction() const;
    v3f get_position() const override {
        return position + get_direction() * -dist;
    }
};

