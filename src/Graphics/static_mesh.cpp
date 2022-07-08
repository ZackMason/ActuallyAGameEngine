// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/static_mesh.hpp"

#include "Util/exceptions.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

namespace internal{
    GLint bound_mesh = {-1};
};

void static_mesh_t::update_aabb() {
    for (const auto& vertex: buffer_object.data) {
        aabb.expand(vertex.position);
    }
}
void static_mesh_t::bind() {
    vertex_array.bind();
}

void static_mesh_t::unbind() {
    vertex_array.unbind();
}

void static_mesh_t::draw() {
    if(index_buffer) {
        vertex_array.bind();
            glDrawElements(vertex_array.topology, index_buffer->data.size(), GL_UNSIGNED_INT, 0);
        //vertex_array.unbind();
    } else {
        vertex_array.draw();
    }
}
