// Author: Zackery Mason-Blaug
// Date: 2022-07-10
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

template <typename T>
struct resource_handle_t {
private:
    static u32& get_dummy_count() {
        static u32 d{0};
        return d;
    }

public:
    resource_handle_t() : resource(nullptr), count(get_dummy_count()) {
    }

    resource_handle_t(T* p_res, u32& p_count) 
    : resource(p_res), count(p_count) 
    {
    }
    ~resource_handle_t() {
        count--;
    }

    resource_handle_t(resource_handle_t& o) 
    : resource(o.resource), count(o.count) 
    {
        count++;
    }
    resource_handle_t(resource_handle_t&& o)
        : resource(o.resource), count(o.count) 
    {
        count++;
    }
    resource_handle_t& operator=(const resource_handle_t& o) {
        resource = o.resource;
        count++;
        return *this;
    };
    resource_handle_t& operator=(resource_handle_t&& o) {
        resource = std::move(o.resource);
        count++;
        return *this;
    };
    
    //operator int() {
    //    return resource->id;
    //}

    operator bool() {
        return valid();
    }

    [[nodiscard]] bool valid() const { return resource != nullptr; }

    [[nodiscard]] T* get_ptr() const {
        return resource;
    } 

    [[nodiscard]] T& get() const { return *resource; }

    [[nodiscard]] const int get_count() const {
        return count;
    } 

private:
    u32& count;
    T* resource;
};


