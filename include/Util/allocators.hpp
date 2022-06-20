// Author: Zackery Mason-Blaug
// Date: 2022-06-19
//////////////////////////////////////////////////////////


#pragma once

#include <stdlib.h> 
#include <iterator> 
#include <array> 
#include <cassert> 

#define ALLOCATOR_EXCEPTIONS 
#ifdef ALLOCATOR_EXCEPTIONS 
#include "Util/exceptions.hpp"
#endif



// will not call destructor/constructor
struct stack_allocator_t {
    using stack_marker_t = size_t;
    explicit stack_allocator_t(size_t stack_size) {
        top = start = reinterpret_cast<char*>(malloc(stack_size));
        end = start + stack_size;
    }

    virtual ~stack_allocator_t() {
        free(start);
    }

    template <typename T>
    T* alloc(size_t count) {
        T* res = reinterpret_cast<T*>(top);
        top += count * sizeof(T);

#ifdef ALLOCATOR_EXCEPTIONS 
        if(top > end) {
            throw runtime_error_x("Stack allocator overflow!");
        }
#endif
        assert(top < end && "Stack allocator overflow!");
        return res;
    }

    void* alloc(size_t size_byte) {
        void* res = reinterpret_cast<void*>(top);
        top += size_byte;
#ifdef ALLOCATOR_EXCEPTIONS 
        if(top > end) {
            throw runtime_error_x("Stack allocator overflow!");
        }
#endif
        assert(top < end && "Stack allocator overflow!");
        return res;
    }

    stack_marker_t get_marker() const {
        return std::distance(start, top);
    }

    void pop(stack_marker_t marker) {
        top = start + marker;
    }

    void clear() {
        top = start;
    }

private:
    char* start{nullptr};
    char* end{nullptr};
    char* top{nullptr};
};

// Page 435 Game Engine Architecture 
template <size_t StackSize>
struct double_stack_allocator_t {
    unsigned char current_stack = 0;
    std::array<stack_allocator_t, 2> stacks{
        stack_allocator_t(StackSize),
        stack_allocator_t(StackSize)
    };

    void swap_buffers() {
        current_stack = current_stack ? 0u : 1u;
        stacks[current_stack].clear();
    }

    void* alloc(size_t size_bytes) {
        return stacks[current_stack].alloc(size_bytes);
    }
};

template <typename T>
struct pool_allocator_t {
    explicit pool_allocator_t(size_t count) {
        top = start = reinterpret_cast<T*>(malloc(sizeof(T)*count));
        end = start + count;
    }

    virtual ~pool_allocator_t() {
        free(start);
    }


    T* alloc(size_t count) {
        T* res = top;
        top += count;

        return res;
    }

    template<typename ... Args>
    T& emplace(Args&& ... args) {
        T* t = new (top++) T(std::forward(args)...); 
        return *t;
    }

private:
    T* start{nullptr};
    T* top{nullptr};
    T* end{nullptr};
};







