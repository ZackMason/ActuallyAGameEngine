// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "entt/entt.hpp"

#include "Engine/window.hpp"
#include "Engine/asset_loader.hpp"
#include "Scripting/script.hpp"

#include "Event/event.hpp"

#include "Util/logger.hpp"

struct application_i {
    window_t window;
    asset_loader_t asset_loader;
    script_vm_t script_vm;
    entt::registry world;

    f32 tick_rate = 1.0f / 60.0f;
    f32 time_scale = 1.0f;

    event_handler_t event_handler;

    virtual void on_event(event_i& event) {
        logger_t::info(fmt::format("Event {}", event.get_type_name()));
    }

    void run() {
        window.open_window();

        window.set_event_callback(std::bind(&application_i::on_event, this, std::placeholders::_1));

        init();

        auto last_time = window.get_ticks();
        auto accum_time = 0.0f;
        while(!window.should_close()) {
            auto time = window.get_ticks();
            auto dt = time_scale * (time - last_time) / tick_rate;
            accum_time += dt;
            last_time = time;
            if (accum_time >= 1.0f) {
                update(tick_rate);
                accum_time -= 1.0f;
            }

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

