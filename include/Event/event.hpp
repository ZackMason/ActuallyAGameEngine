// Author: Zackery Mason-Blaug
// Date: 2022-06-19
//////////////////////////////////////////////////////////


#pragma once

#include <array>
#include <functional>

#include "types.hpp"
#include "Util/string.hpp"
#include "Util/vector.hpp"

#define MAKE_EVENT_TYPE(x) \
    constexpr static event_type_t get_static_type() { return sid(#x); } \
    virtual event_type_t get_type() const override { return get_static_type(); }\
    const char* get_type_name() const override {            \
        return #x;                                          \
    }                                                       \

struct event_i {
    using event_type_t = string_id_t;

    virtual const char* get_type_name() const = 0;
    virtual event_type_t get_type() const = 0;
};

struct event_handler_t {
    using event_callback = std::function<bool(const event_i&)>;
    
    
    //void register_event(string_id_t event_type, std::function<bool(const event_i&)> callback) const {
    //    observers[event_type].push_back(callback);
    //}

    template<typename E, typename Fn>
    bool dispatch(event_i& event, Fn f) const {
        const auto type = event.get_type();

        if (type == E::get_static_type()) {
            E& typed_event = static_cast<E&>(event);
            if (f(typed_event)) {
                return true;
            }
        }
        return false;

#if 0
        if (observers.find(type) == observers.end())
            return false;

        auto&& type_observers = observers.at(type);
        for(auto&& observer: type_observers) {
            if (observer(event)) {
                return true;
            }
        } 
        return false;
#endif
    }

private:
    //mutable std::unordered_map<string_id_t, utl::vector<event_callback>> observers;
};