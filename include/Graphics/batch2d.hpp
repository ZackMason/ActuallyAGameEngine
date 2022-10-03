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
#include "Util/logger.hpp"

#include "Util/color.hpp"

#include "Math/aabb.hpp"

#include <atomic>

template <size_t Count>
struct batch2d_t {
    struct vertex2d_t {
        v2f position{};
        v2f uv{};
        int tex{};
    };

    mapped_buffer_t<vertex2d_t, 4 * Count> vertices;
    mapped_buffer_t<unsigned int, 6 * Count> indices;
    vertex_array_t vertex_array;
    std::vector<resource_handle_t<texture2d_t>> textures;
    v2f screen_size{};
    std::atomic_int quad_count{0};
    resource_handle_t<shader_t> shader;

private:
    vertex2d_t* vertex_ptr{nullptr};
public:

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

    void draw_ex(
        const color32 color,
        const v2f& center,
        const v2f& size,
        const f32 angle
    ) {
        add_quad_ex(
            (v2f{center.x*2, screen_size.y} - center) / screen_size,
            size, 
            angle, 
            color
        );
    }

    void draw(
        const color32 color,
        const v2f& position,
        const v2f& size
    ) {
        // logger_t::info(fmt::format("Drawing color: {:x}", static_cast<int>(color)));

        add_quad(
            (v2f{position.x*2, screen_size.y} - position) / screen_size, 
            size / screen_size, 
            static_cast<int>(color)
        );
    }
    
    void draw(
        const color32 color,
        const aabb_t<v2f>& box
    ) {
        draw(color, box.min, box.size());
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

        const int i = static_cast<int>((quad_count++) * 4);

        vertex_ptr[i+0] = (vertex2d_t{center + up - r, v2f{0,1}, texture_id});
        vertex_ptr[i+2] = (vertex2d_t{center + up + r, v2f{1,1}, texture_id});
        vertex_ptr[i+1] = (vertex2d_t{center - up - r, v2f{0,0}, texture_id});
        vertex_ptr[i+3] = (vertex2d_t{center - up + r, v2f{1,0}, texture_id});
    }


    void add_quad(const aabb_t<v2f>& box, int texture_id) {
        add_quad(box.min, box.min + box.size(), texture_id);
    }

    void add_quad(const v2f& position, const v2f& size, int texture_id) {
        const int i = static_cast<int>((quad_count++) * 4);

        vertex_ptr[i]   = (vertex2d_t{position,                    v2f{0, 1}, texture_id});
        vertex_ptr[i+1] = (vertex2d_t{position + v2f{size.x, 0},   v2f{1, 1}, texture_id});
        vertex_ptr[i+2] = (vertex2d_t{position + v2f{0, -size.y},  v2f{0, 0}, texture_id});
        vertex_ptr[i+3] = (vertex2d_t{position + size * v2f{1,-1}, v2f{1, 0}, texture_id});
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

    void present() {
        if (!quad_count) return;

        assert(shader.valid() && "batch2d:: shader is invalid");
        shader.get().bind();

        int s[32];
	    for (int i = 0; i < 32; i++) s[i] = i;
        shader.get().set_intv("uTextures", s, 32);
        
        for (auto i{ 0 }; auto & texture : textures) {
            texture.get().bind_slot(i++);
        }

        vertex_array.draw_elements(quad_count*6);
    }

    void add_texture(resource_handle_t<texture2d_t> texture) {
        textures.push_back(texture);
    }

    void clear() {
        if (quad_count || !vertex_ptr) {
            // dont need to sync if we didnt write to the vertices last frame
            vertex_ptr = vertices.get_data();
        }
        vertex_array.size = quad_count = 0;
    }
    
    void set_indices() {
        unsigned int* data = indices.get_data();
        size_t w = 0;
        for (int i = 0; i < vertices.count(); i+=4) {
            data[w++] = i;
            data[w++] = i + 2;
            data[w++] = i + 1;

            data[w++] = i + 1;
            data[w++] = i + 2;
            data[w++] = i + 3;
        }
    }

    explicit batch2d_t()
    : indices(GL_ELEMENT_ARRAY_BUFFER), vertex_array(vertices.id, indices.id, 6000, sizeof(vertex2d_t)) {  
        set_indices();

        vertex_array
            .set_attrib(0, 2, GL_FLOAT, offsetof(vertex2d_t, position))
            .set_attrib(1, 2, GL_FLOAT, offsetof(vertex2d_t, uv))
            .set_attribi(2, 1, GL_INT, offsetof(vertex2d_t, tex));
    }
};

