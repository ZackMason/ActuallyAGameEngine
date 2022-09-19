// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Util/vector.hpp"

#include "Math/aabb.hpp"

#include "Graphics/vertex_array.hpp"
#include "Graphics/buffer.hpp"

#include <optional>

struct static_vertex_t {
    v3f position;
    v3f normal;
    v2f tex_coord;
};

struct static_mesh_t : drawable_i {
    std::optional<buffer_t<utl::vector<unsigned int>>> index_buffer;
    buffer_t<utl::vector<static_vertex_t>> buffer_object;
    vertex_array_t vertex_array;

    aabb_t<v3f> aabb;

    void update_aabb();

    void bind() override;
    void unbind() override;
    void draw() override;

    explicit static_mesh_t(const utl::vector<static_vertex_t>& p_vertices)
        : buffer_object(p_vertices), vertex_array(buffer_object.size()), aabb{}
    {
        buffer_object.bind();

        vertex_array.bind_ref()
            .set_attrib(0, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, normal))
            .set_attrib(2, 2, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, tex_coord))
            .unbind();
        buffer_object.unbind();

        update_aabb();
    }

    explicit static_mesh_t(utl::vector<static_vertex_t>&& p_vertices) 
        : buffer_object(std::move(p_vertices)), vertex_array(buffer_object.size()), aabb{}
    { 
        buffer_object.bind();
        vertex_array.bind_ref()
            .set_attrib(0, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, normal))
            .set_attrib(2, 2, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, tex_coord))
            .unbind();
        buffer_object.unbind();

        update_aabb();
    }

    explicit static_mesh_t(utl::vector<unsigned int>&& p_indices, utl::vector<static_vertex_t>&& p_vertices) 
        :  buffer_object(std::move(p_vertices)), 
        vertex_array(buffer_object.size()), aabb{}
    { 
        vertex_array.bind();
        buffer_object.bind();
        index_buffer.emplace(std::move(p_indices), GL_ELEMENT_ARRAY_BUFFER);
        index_buffer->bind();
        vertex_array.bind_ref()
            .set_attrib(0, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, normal))
            .set_attrib(2, 2, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, tex_coord))
            .unbind();
        buffer_object.unbind();
        index_buffer->unbind();

        update_aabb();
    }

    static_mesh_t() = default;

    static_mesh_t(static_mesh_t&) = delete;
    static_mesh_t(static_mesh_t&&) = delete;
    static_mesh_t& operator=(static_mesh_t&) = delete;
    static_mesh_t& operator=(static_mesh_t&&) = delete;
};

