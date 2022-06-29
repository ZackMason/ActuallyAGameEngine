// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "entt/entt.hpp"

#include "Engine/window.hpp"
#include "Engine/asset_loader.hpp"
#include "Scripting/script.hpp"


struct application_i {
    window_t window;
    asset_loader_t asset_loader;
    script_vm_t script_vm;
    entt::registry world;

    void run() {
        window.open_window();
        init();

        auto last_time = window.get_ticks();
        while(!window.should_close()) {
            auto time = window.get_ticks();
            auto dt = (time - last_time);
            last_time = time;
            update(dt);

            pre_render_pass();
            main_render_pass();
            post_process_pass();

            window.swap_buffers();
            window.poll_events();
        }

        close();
    }
    virtual void init(){};
    virtual void pre_render_pass(){};
    virtual void main_render_pass(){};
    virtual void post_process_pass(){};
    virtual void update(f32 dt){};

    virtual void close(){
        world.clear();
    };
};

