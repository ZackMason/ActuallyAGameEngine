// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Util/vector.hpp"

#include "Graphics/vertex_array.hpp"
#include "Graphics/buffer.hpp"

struct static_vertex_t {
    v3f position;
    v3f normal;
    v2f tex_coord;
};

struct static_mesh_t : drawable_i {
    buffer_t<static_vertex_t> buffer_object;
    vertex_array_t vertex_array;

    void bind() override;
    void unbind() override;
    void draw() override;

    static static_mesh_t from_gltf(const std::string& path);
    static static_mesh_t from_obj(const std::string& path);
    static void emplace_obj(const std::string& path, static_mesh_t* address);

    static_mesh_t(utl::vector<static_vertex_t>&& p_vertices) 
        : buffer_object(std::move(p_vertices)), vertex_array(buffer_object.size())
    { 
        buffer_object.bind();
        vertex_array.bind_ref()
            .set_attrib(0, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, normal))
            .set_attrib(2, 2, GL_FLOAT, sizeof(static_vertex_t), offsetof(static_vertex_t, tex_coord))
            .unbind();
        buffer_object.unbind();
    }
};

