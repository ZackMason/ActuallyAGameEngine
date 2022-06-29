// Author: Zackery Mason-Blaug
// Date: 2022-06-29
//////////////////////////////////////////////////////////


#include "Graphics/line_renderer.hpp"

utl::vector<line_point_t> construct_aabb(const aabb_t<v3f>& aabb, v4f color) {
    const auto size = aabb.max - aabb.min;
    const v3f sx = {size.x, 0, 0};
    const v3f sy = {0, size.y, 0};
    const v3f sz = {0, 0, size.z};
    return {
        line_point_t{aabb.min, color}, line_point_t{aabb.min + sx ,color},
        line_point_t{aabb.min, color}, line_point_t{aabb.min + sy ,color},
        line_point_t{aabb.min, color}, line_point_t{aabb.min + sz ,color},

        line_point_t{aabb.min + sx + sy, color}, line_point_t{aabb.min + sx ,color},
        line_point_t{aabb.min + sx + sy, color}, line_point_t{aabb.min + sy ,color},
        line_point_t{aabb.max - sx - sy, color}, line_point_t{aabb.max - sy ,color},
        line_point_t{aabb.max - sx - sy, color}, line_point_t{aabb.max - sx ,color},

        line_point_t{aabb.min + sx, color}, line_point_t{aabb.min + sx + sz, color},
        line_point_t{aabb.min + sy, color}, line_point_t{aabb.min + sy + sz, color},

        line_point_t{aabb.max, color}, line_point_t{aabb.max - sx ,color},
        line_point_t{aabb.max, color}, line_point_t{aabb.max - sy ,color},
        line_point_t{aabb.max, color}, line_point_t{aabb.max - sz ,color},
    };
}

void line_renderer_t::update_aabb() {
    for(const auto& [pos, color]: buffer_object.data) {
        aabb.expand(pos);
    }
}
void line_renderer_t::bind() {
    vertex_array.bind();
}
void line_renderer_t::unbind() {
    vertex_array.unbind();
}
void line_renderer_t::draw() {
    vertex_array.draw();
}

