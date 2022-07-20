// Author: Zackery Mason-Blaug
// Date: 2022-07-10
//////////////////////////////////////////////////////////


#pragma once

#include <array>

#include "types.hpp"

#include "Util/vector.hpp"

#include "Graphics/vertex_array.hpp"
#include "Graphics/buffer.hpp"

struct particle_t {
    v3f position{};
    v3f velocity{};
    v4f color{1.0f};
};

struct particle_attractor_t {
    v3f position{};
    f32 strength{};
};

template <size_t N>
struct particle_array_t {
    std::array<particle_t, N> particles;
    std::array<f32, N> life_time;

    utl::vector<particle_t> get_active() const {
        utl::vector<particle_t> res;
        res.reserve(N);
        for (auto i{0}; i < N; i++) {
            if (life_time[i] > 0.0f) {
                res.push_back(particles[i]);
            }
        }
        return res;
    }

    void emit(particle_t particle, f32 life) {
        for (auto i{0}; i < N; i++) {
            if (life_time[i] <= 0.0f) {
                particles[i] = particle;
                life_time[i] = life;
                return;
            }
        }
    }

    void update(f32 dt, 
        const v3f gravity = v3f{0.0f, -9.81f, 0.0f}, 
        const utl::vector<particle_attractor_t>& attractors = {}
    ) {
        for (auto i{0}; i < N; i++) {
            auto& time = life_time[i];
            auto& p = particles[i];

            const auto on = f32(time>0.0f);
            time -= dt * on;
            p.velocity += gravity * dt * on;
            for (const auto& attractor: attractors) {
                const auto to_attractor = (attractor.position - p.position);
                p.velocity +=  glm::normalize(to_attractor) * attractor.strength / glm::length(to_attractor) * dt * on;
            }
            p.position += p.velocity * dt * on;
            
            //particles[i].velocity = on * damp(particles[i].velocity, v3f{0.0f}, 0.5f, dt);
        }
    }
};

struct particle_system_t {
    particle_array_t<100> particles;
    buffer_t<utl::vector<particle_t>> buffer_object;
    vertex_array_t vertex_array;

    void bind() {
        vertex_array.bind();
    }

    void unbind() {
        vertex_array.unbind();
    }

    void update_buffer() {
        bind();
        buffer_object.bind();
        buffer_object.data = particles.get_active();
        vertex_array.size = static_cast<int>(buffer_object.data.size());
        buffer_object.update_buffer();    
    }

    void draw() {
        bind();
        vertex_array.draw();
    }

    void emit(particle_t particle, f32 life) {
        particles.emit(particle, life);
    }

    particle_system_t() :
        buffer_object(utl::vector<particle_t>{100, particle_t{}}), 
        vertex_array(buffer_object.size())
    {
        vertex_array.topology = GL_POINTS;
        buffer_object.bind();
        vertex_array.bind_ref()
            .set_attrib(0, 3, GL_FLOAT, sizeof(particle_t), offsetof(particle_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(particle_t), offsetof(particle_t, velocity))
            .set_attrib(2, 4, GL_FLOAT, sizeof(particle_t), offsetof(particle_t, color))
            .unbind();
        buffer_object.unbind();
    }
};

