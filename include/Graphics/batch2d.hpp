// Author: Zackery Mason-Blaug
// Date: 2022-09-18
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include "Graphics/buffer.hpp"
#include "Graphics/shader.hpp"
#include "Graphics/vertex_array.hpp"
#include "Graphics/texture2d.hpp"

#include "Engine/resource_handle.hpp"

#include "Util/color.hpp"

#include "Math/aabb.hpp"

struct batch2d_t {
    struct vertex2d_t {
        v2f position{};
        v3f uv{};
    };

    buffer_t<std::vector<vertex2d_t>> vertices{};
    buffer_t<std::vector<unsigned int>> indices{};
    vertex_array_t vertex_array;
    std::vector<resource_handle_t<texture2d_t>> textures;
    v2f screen_size{};
    size_t quad_count{0};

    void draw(resource_handle_t<texture2d_t> texture, const v2f& position) {
        add_quad( 
            (v2f{position.x*2, screen_size.y} - position) / screen_size, 
            v2f{texture.get().width, texture.get().height} / screen_size, 
            get_texture(texture)
        );
    }

    void draw(resource_handle_t<texture2d_t> texture, const v2f& position, const v2f& size) {
        add_quad( 
            (v2f{position.x*2, screen_size.y} - position) / screen_size, 
            size / screen_size, 
            get_texture(texture)
        );
    }

    void draw_scaled(resource_handle_t<texture2d_t> texture, const v2f& position, const v2f& scale) {
        add_quad( 
            (v2f{position.x*2, screen_size.y} - position) / screen_size, 
            v2f{texture.get().width, texture.get().height} / screen_size * scale, 
            get_texture(texture)
        );
    }

    void draw_ex(
        const resource_handle_t<texture2d_t> texture,
        const v2f& center,
        const f32 angle
    ) {
        draw_ex(
            texture,
            center,
            texture.get().get_image_size(),
            angle
        );
    }

    void draw_ex(
        const resource_handle_t<texture2d_t> texture,
        const v2f& center,
        const v2f& size,
        const f32 angle
    ) {
        add_quad_ex(
            (v2f{center.x*2, screen_size.y} - center) / screen_size,
            size, 
            angle, 
            get_texture(texture)
        );
    }

    void draw(
        const color32 color,
        const v2f& position,
        const v2f& size
    ) {
        add_quad(position, size, static_cast<int>(color));
    }

    void add_quad_ex(
        const v2f& center, 
        const v2f& size, 
        f32 angle, 
        int texture_id
    ) {
        const v2f h_size = size * 0.5f;
        const v2f right = v2f{glm::cos(angle), glm::sin(angle)};
        const v2f up = v2f{right.y, -right.x} * h_size.y / screen_size;
        const v2f r = right * h_size.x / screen_size;

        const int i = static_cast<int>(vertices.size());

        vertices.data.push_back(vertex2d_t{center + up - r, v3f{0,1,texture_id}});
        vertices.data.push_back(vertex2d_t{center + up + r, v3f{1,1,texture_id}});
        vertices.data.push_back(vertex2d_t{center - up - r, v3f{0,0,texture_id}});
        vertices.data.push_back(vertex2d_t{center - up + r, v3f{1,0,texture_id}});

        indices.data.push_back(i);
        indices.data.push_back(i + 1);
        indices.data.push_back(i + 2);

        indices.data.push_back(i + 1);
        indices.data.push_back(i + 3);
        indices.data.push_back(i + 2);

        vertices.update_buffer();
        indices.update_buffer();
        vertex_array.size = static_cast<GLsizei>(indices.size());

        quad_count += 1;
    }

    void add_quad(const aabb_t<v2f>& box, int texture_id) {
        add_quad(box.min, box.min + box.size(), texture_id);
    }

    void add_quad(const v2f& position, const v2f& size, int texture_id) {
        const int i = static_cast<int>(vertices.size());

        vertices.data.push_back(vertex2d_t{position, v3f{0, 1, texture_id}});
        vertices.data.push_back(vertex2d_t{position + v2f{size.x, 0}, v3f{1, 1, texture_id}});
        vertices.data.push_back(vertex2d_t{position + v2f{0, -size.y}, v3f{0, 0, texture_id}});
        vertices.data.push_back(vertex2d_t{position + size * v2f{1,-1}, v3f{1, 0, texture_id}});
        
        indices.data.push_back(i);
        indices.data.push_back(i + 2);
        indices.data.push_back(i + 1);

        indices.data.push_back(i + 1);
        indices.data.push_back(i + 2);
        indices.data.push_back(i + 3);

        vertices.update_buffer();
        indices.update_buffer();
        vertex_array.size = static_cast<GLsizei>(indices.size());

        quad_count += 1;
    }

    int get_texture(resource_handle_t<texture2d_t> texture) {
        for (auto i{0}; const auto& t: textures) {
            if (t.get().id == texture.get().id) {
                return i;
            }
            i++;
        }
        add_texture(texture);
        return get_texture(texture);
        return -1;
    }

    void present(resource_handle_t<shader_t> shader) {
        shader.get().bind();

        int s[32];
	    for (int i = 0; i < 32; i++) s[i] = i;
        shader.get().set_intv("uTextures", s, 32);
        
        for (auto i{ 0 }; auto & texture : textures) {
            texture.get().bind_slot(i++);
            texture.get().bind();
        }

        vertex_array.draw(indices);
    }

    void add_texture(resource_handle_t<texture2d_t> texture) {
        textures.push_back(texture);
    }

    void clear() {
        vertices.data.clear();
        indices.data.clear();
        quad_count = 0;
    }

    explicit batch2d_t()
    : vertices(
        []() {
            std::vector<vertex2d_t> v;
            v.reserve(4000);
            v.resize(4000);
            std::fill(v.begin(), v.end(), vertex2d_t{});
            return v;
        }()
    ), indices([]() {
            std::vector<unsigned int> v;
            v.reserve(6000);
            v.resize(6000);
            std::fill(v.begin(), v.end(), 0);
            return v;
        }()), vertex_array(6000) {
        indices.type = GL_ELEMENT_ARRAY_BUFFER;
        vertex_array.size = 0;
        
        vertex_array.bind();
        vertices.create();
        indices.create();

        vertices.data.clear();
        indices.data.clear();

        vertices.bind();
        indices.bind();

        vertex_array.bind_ref()
            .set_attrib(0, 2, GL_FLOAT, sizeof(vertex2d_t), offsetof(vertex2d_t, position))
            .set_attrib(1, 3, GL_FLOAT, sizeof(vertex2d_t), offsetof(vertex2d_t, uv))
            .unbind();

        vertices.unbind();
        indices.unbind();
    }
};

