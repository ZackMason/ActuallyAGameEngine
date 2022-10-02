// Author: Zackery Mason-Blaug
// Date: 2022-06-29
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Graphics/vertex_array.hpp"
#include "Graphics/buffer.hpp"

#include "Util/vector.hpp"

#include "Math/aabb.hpp"

struct line_point_t {
    v3f position;
    v4f color;
};

utl::vector<line_point_t> construct_aabb(const aabb_t<v3f>& aabb, v4f color);

struct line_renderer_t : drawable_i {
    buffer_t<utl::vector<line_point_t>> buffer_object;
    vertex_array_t vertex_array;
    aabb_t<v3f> aabb;

    void update_aabb();
    void bind() override;
    void unbind() override;
    void draw() override;

    explicit line_renderer_t(utl::vector<line_point_t>&& p_vertices) 
        : buffer_object(std::move(p_vertices)),
        vertex_array(buffer_object.id, buffer_object.size(), sizeof(line_point_t)), 
        aabb{v3f{0.0f}, v3f{0.0f}}
    { 
        vertex_array.topology = GL_LINES;
        
        vertex_array
            .set_attrib(0, 3, GL_FLOAT, offsetof(line_point_t, position))
            .set_attrib(1, 4, GL_FLOAT, offsetof(line_point_t, color));
        
        update_aabb();
    }

    line_renderer_t(line_renderer_t&) = delete;
    line_renderer_t(line_renderer_t&&) = delete;
    line_renderer_t& operator=(line_renderer_t&) = delete;
    line_renderer_t& operator=(line_renderer_t&&) = delete;
};

