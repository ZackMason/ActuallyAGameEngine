// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////

#include <GLFW/glfw3.h>

#include "Engine/orbit_fly_camera.hpp"

void orbit_fly_camera_t::update(const window_t& window, f32 dt) {
    orbit_camera.width = fly_camera.width = (f32)window.width;
    orbit_camera.height = fly_camera.height = (f32)window.height;
    if (is_fly) {
        orbit_camera.position = fly_camera.position;
        orbit_camera.lastx = fly_camera.lastx;
        orbit_camera.lasty = fly_camera.lasty;

    } else {
        fly_camera.position = orbit_camera.position;
        fly_camera.lastx = orbit_camera.lastx;
        fly_camera.lasty = orbit_camera.lasty;
    }

    if (window.is_button_pressed(GLFW_MOUSE_BUTTON_2)) {
        orbit_camera.update(window, dt);
        is_fly = false;
    } else {
        fly_camera.update(window, dt);
        is_fly = true;
    }
}