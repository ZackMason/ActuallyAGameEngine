// Author: Zackery Mason-Blaug
// Date: 2022-08-01
//////////////////////////////////////////////////////////


#pragma once

#include "Event\event.hpp"

struct native_script_i {
    virtual void update(float dt){}
    virtual void on_create(){}
    virtual void on_destroy(){}
    virtual void on_event(event_i& event) {}

    native_script_i() = default;    
    virtual ~native_script_i() = default;
};

struct native_script_c {
    native_script_i* script{nullptr};

    void update(float dt) {
        assert(script);
        script->update(dt);
    }

    void on_create() {
        assert(script); 
        script->on_create();
    }

    void on_destroy() {
        assert(script);
        script->on_destroy();
    }

    void on_event(event_i& event) {
        assert(script);
        script->on_event(event);
    }

    explicit native_script_c(native_script_i* const _script) 
        : script(_script)
    {
        on_create();
    }

    virtual ~native_script_c() {
        on_destroy();

        delete script;
        script = nullptr;
    }
};