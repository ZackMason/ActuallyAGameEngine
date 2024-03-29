// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include <utility>
#include <cassert>

#include "Util/vector.hpp"
#include "Graphics/bindable.hpp"



template <typename T>
struct buffer_t : bindable_i {
    GLenum type = GL_ARRAY_BUFFER;
    T data;

    auto size() const {
        return data.size();
    }

    void bind() {
        glBindBuffer(type, id);
    }

    void unbind() {
        glBindBuffer(type, 0);
    }

    void create() {
        glCreateBuffers(1, &id);
                
        if (data.size()) {
            glNamedBufferStorage(id, data.size() * sizeof(typename T::value_type), data.data(), GL_DYNAMIC_STORAGE_BIT);
        }
    }

    // use mapped_buffer_t instead if you need to update regularly
    void update_buffer() {
        glNamedBufferSubData(id, 0, data.size() * sizeof(typename T::value_type), data.data());
    }

    // only call if you are going to create a new buffer
    void destroy() {
        glDeleteBuffers(1, &id);
    }

    explicit buffer_t(T&& p_data, GLenum p_type)
        : type(p_type), data(std::move(p_data))
    {
        create();
    }

    explicit buffer_t(const T& p_data, GLenum p_type)
        : type(p_type), data(p_data)
    {
        create();
    }

    explicit buffer_t(T&& p_data)
        : data(std::move(p_data))
    {
        create();
    }

    explicit buffer_t(const T& p_data)
        : data(p_data)
    {
        create();
    }

    virtual ~buffer_t() {
        destroy();
    }

    //buffer_t() = default;
    buffer_t(buffer_t&) = delete;
    buffer_t(buffer_t&&) = delete;
    buffer_t& operator=(buffer_t&) = delete;
    buffer_t& operator=(buffer_t&&) = delete;
};

// for multi threaded writing
template <typename T, size_t Count>
struct mapped_buffer_t : gl_handle_t {
    GLenum type = GL_ARRAY_BUFFER; 
    GLsync sync_id;

    auto size() const {
        return Count * sizeof(T);
    }

    auto count() const {
        return Count;
    }

    void wait() {
        if (glIsSync(sync_id)) {
            GLenum wait_return = GL_UNSIGNALED;
            while(wait_return != GL_ALREADY_SIGNALED && wait_return != GL_CONDITION_SATISFIED)
            {
                wait_return = glClientWaitSync(sync_id, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
            }
    
            glDeleteSync(sync_id);
            sync_id = 0;
        }
    }

    void sync() {
        sync_id = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    [[nodiscard]] T* get_data() {
        wait();
        
        return data;
    }

    void create() {
        glCreateBuffers(1, &id);

        const auto flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(id, Count * sizeof(T), 0, flags);
        data = static_cast<T*>(glMapNamedBufferRange(id, 0, Count * sizeof(T), flags));
        assert(data);
    }

    // only call if you are going to create a new buffer
    void destroy() {
        glDeleteBuffers(1, &id);
    }

    explicit mapped_buffer_t(GLenum p_type)
        : type(p_type)
    {
        create();
    }

    mapped_buffer_t() {
        create();
    }

    virtual ~mapped_buffer_t() {
        destroy();
    }
    mapped_buffer_t(mapped_buffer_t&) = delete;
    mapped_buffer_t(mapped_buffer_t&&) = delete;
    mapped_buffer_t& operator=(mapped_buffer_t&) = delete;
    mapped_buffer_t& operator=(mapped_buffer_t&&) = delete;
    
private:
    T* data{nullptr};
};

template <typename T>
struct mapped_dyn_buffer_t : gl_handle_t {
    GLenum type = GL_ARRAY_BUFFER; 
    GLsync sync_id;
    size_t capacity{256};
    size_t count{0};

    T& push_back(const T& x) {
        if (count == capacity) {
            resize(capacity<<1);
        }
        return data_ptr[count++] = x;
    }

    auto size() const {
        return count;
    }

    void clear() const {
        count = 0;
    }

    [[nodiscard]] bool empty() const {
        return count == 0;
    }
    
    void resize(size_t new_size) {

        T* temp_data = new T[count];
        std::memcpy(temp_data, data_ptr, sizeof(T) * count);

        count = 0;
        capacity = new_size;
        const auto flags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
        glNamedBufferStorage(id, capacity * sizeof(T), nullptr, flags);
        data_ptr = static_cast<T*>(glMapNamedBufferRange(id, 0, capacity * sizeof(T), flags));

        std::memcpy(data_ptr, temp_data, sizeof(T) * count);

        delete[] temp_data;
    }

    void wait() {
        if (glIsSync(sync_id)) {
            GLenum wait_return = GL_UNSIGNALED;
            while(wait_return != GL_ALREADY_SIGNALED && wait_return != GL_CONDITION_SATISFIED)
            {
                wait_return = glClientWaitSync(sync_id, GL_SYNC_FLUSH_COMMANDS_BIT, 1);
            }
    
            glDeleteSync(sync_id);
            sync_id = 0;
        }
    }

    void sync() {
        sync_id = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    }

    [[nodiscard]] T* data() {
        wait();
        
        return data;
    }

    void create() {
        glCreateBuffers(1, &id);

        resize(capacity);

        assert(data_ptr);
    }

    // only call if you are going to create a new buffer
    void destroy() {
        glDeleteBuffers(1, &id);
    }

    T& operator[](size_t i) {
        assert(i<count);
        return data_ptr[i];
    }


    explicit mapped_dyn_buffer_t(GLenum p_type)
        : type(p_type)
    {
        create();
    }

    mapped_dyn_buffer_t() {
        create();
    }

    virtual ~mapped_dyn_buffer_t() {
        destroy();
    }
    mapped_dyn_buffer_t(mapped_dyn_buffer_t&) = delete;
    mapped_dyn_buffer_t(mapped_dyn_buffer_t&&) = delete;
    mapped_dyn_buffer_t& operator=(mapped_dyn_buffer_t&) = delete;
    mapped_dyn_buffer_t& operator=(mapped_dyn_buffer_t&&) = delete;
    
private:
    T* data_ptr{nullptr};
};

