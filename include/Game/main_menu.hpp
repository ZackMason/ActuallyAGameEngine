// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "Engine/application.hpp"

struct main_menu_t : application_i {
    void init() override;
    void pre_render_pass() override;
    void main_render_pass() override;
    void post_process_pass() override;
    void update() override;

    bool is_server{false};

    bool run_menu() {
        run();
        return is_server;
    }

    void close() override;
};

