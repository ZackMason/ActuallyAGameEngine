// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/static_mesh.hpp"

#include "Util/exceptions.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"


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
        vertex_array.unbind();
    } else {
        vertex_array.draw();
    }
}

utl::vector<static_vertex_t> vertices_from_obj_mesh(
    const tinyobj::mesh_t& mesh, 
    const tinyobj::attrib_t& attrib
    ) {
    size_t index_offset = 0;
    utl::vector<static_vertex_t> vertices;
    for (size_t f = 0; f < mesh.num_face_vertices.size(); f++)
    {
        int fv = mesh.num_face_vertices[f];
        
        for (int v = 0; v < fv; v++) {
            // access to vertex
            tinyobj::index_t idx = mesh.indices[index_offset + v];
            tinyobj::real_t vx = attrib.vertices[3 * idx.vertex_index + 0];
            tinyobj::real_t vy = attrib.vertices[3 * idx.vertex_index + 1];
            tinyobj::real_t vz = attrib.vertices[3 * idx.vertex_index + 2];
            tinyobj::real_t nx = attrib.normals[3 * idx.normal_index + 0];
            tinyobj::real_t ny = attrib.normals[3 * idx.normal_index + 1];
            tinyobj::real_t nz = attrib.normals[3 * idx.normal_index + 2];
            tinyobj::real_t tx = attrib.texcoords[2 * idx.texcoord_index + 0];
            tinyobj::real_t ty = attrib.texcoords[2 * idx.texcoord_index + 1];
            
            static_vertex_t vert;
            vert.position = { vx,vy,vz };
            vert.normal = { nx,ny,nz };
            vert.tex_coord = { tx,ty };
            
            vertices.push_back(vert);
        }
        
#if 0                
        auto iv1 = vertices.size() - 3;
        v3f v1 = vertices[iv1 + 0].Position;
        v3f v2 = vertices[iv1 + 1].Position;
        v3f v3 = vertices[iv1 + 2].Position;
        v2f uv1 = vertices[iv1 + 0].TexCoords;
        v2f uv2 = vertices[iv1 + 1].TexCoords;
        v2f uv3 = vertices[iv1 + 2].TexCoords;
        
        auto e1 = v2 - v1;
        auto e2 = v3 - v1;
        
        auto duv1 = uv2 - uv1;
        auto duv2 = uv3 - uv1;
        
        float det = 1.0f / (duv1.x * duv2.y - duv2.x * duv1.y);
        
        v3f t, b;
        
        t.x = det * (duv2.y * e1.x - duv1.y * e2.x);
        t.y = det * (duv2.y * e1.y - duv1.y * e2.y);
        t.z = det * (duv2.y * e1.z - duv1.y * e2.z);
        t = normalize(t);
        
        b.x = det * (-duv2.x * e1.x + duv1.x * e2.x);
        b.y = det * (-duv2.x * e1.y + duv1.x * e2.y);
        b.z = det * (-duv2.x * e1.z + duv1.x * e2.z);
        b = normalize(b);
        
        vertices[iv1 + 0].Tangent = t;
        vertices[iv1 + 1].Tangent = t;
        vertices[iv1 + 2].Tangent = t;
        
        vertices[iv1 + 0].Bitangent = b;
        vertices[iv1 + 1].Bitangent = b;
        vertices[iv1 + 2].Bitangent = b;
#endif
        
        index_offset += fv;
    }
    return vertices;
}

utl::vector<static_vertex_t> load_obj(const std::string& path, const std::string& asset_dir) {
    utl::vector<static_vertex_t> vertices;
    std::vector<unsigned int> indices;
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    
    std::string err;
    bool ret = tinyobj::LoadObj(
        &attrib, 
        &shapes, 
        &materials, 
        &err, 
        fmt::format("{}{}", asset_dir, path).c_str(), 
        asset_dir.c_str(), 
        true);

    if (!ret) {
        throw runtime_error_x(fmt::format("Static mesh: {} - {}", path, err));
    }

    for (size_t s = 0; s < shapes.size(); s++)
    {
        // Loop over faces(polygon)
        auto mesh_verts = vertices_from_obj_mesh(shapes[s].mesh, attrib);
        vertices.insert(vertices.end(), 
            std::make_move_iterator(mesh_verts.begin()),
            std::make_move_iterator(mesh_verts.end()));
    }
    return vertices;
}

static_mesh_t static_mesh_t::from_obj(const std::string& path,const std::string& asset_dir) {
    return static_mesh_t(std::move(load_obj(path,asset_dir)));
}

void static_mesh_t::emplace_obj(const std::string& path, static_mesh_t* address,const std::string& asset_dir) {
    new (address) static_mesh_t(std::move(load_obj(path,asset_dir)));
}
