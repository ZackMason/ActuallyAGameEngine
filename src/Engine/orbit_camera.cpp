// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#include <GLFW/glfw3.h>

#include "Engine/orbit_camera.hpp"

void orbit_camera_t::update(const window_t& window, f32 dt) {
    width = (f32)window.width;
    height = (f32)window.height;

    constexpr f32 move_speed = 15.0f;
    constexpr f32 camera_look_speed = 5.0f;
    if (window.is_key_pressed(GLFW_KEY_W)) {
        position += forward() * dt * move_speed;
    }
    if (window.is_key_pressed(GLFW_KEY_S)) {
        position += back() * dt * move_speed;
    }
    if (window.is_key_pressed(GLFW_KEY_D)) {
        position += right() * dt * move_speed;
    }
    if (window.is_key_pressed(GLFW_KEY_A)) {
        position += left() * dt * move_speed;
    }
    if (window.is_key_pressed(GLFW_KEY_Q)) {
        position += up() * dt * move_speed;
    }
    if (window.is_key_pressed(GLFW_KEY_E)) {
        position += down() * dt * move_speed;
    }




    auto [x,y] = window.get_mouse();
    auto [sx,sy] = window.get_scroll();
    auto [dx, dy] = std::array<f32,2>{lastx-x, lasty-y};

    dist = std::max(dist + -sy * dt, 1.0f);

    if (window.is_button_pressed(GLFW_MOUSE_BUTTON_2)) {
        yaw += -dx * dt  * camera_look_speed;
        pitch += dy * dt * camera_look_speed;
    }
    v3f direction{0,0,-1};
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

    const auto temp = position;
    position += direction * -dist;
    look_at(temp);

    position = temp;

    make_projection();
    

    lastx = x; lasty = y;
}
