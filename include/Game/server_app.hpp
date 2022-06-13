// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "Engine/application.hpp"

#include "Engine/orbit_fly_camera.hpp"

#include "Game/game_server.hpp"
#include "Game/chat_log.hpp"

struct server_app_t : application_i {
    game_server_t game_server;
    chat_log_t chat_log;

    orbit_fly_camera_t camera{45.0f, 100.0f, 100.0f, 0.01f, 400.0f};

    void init() override;
    void pre_render_pass() override;
    void main_render_pass() override;
    void post_process_pass() override;
    void update() override;

    void close() override;
};

