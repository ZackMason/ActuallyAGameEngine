// Author: Zackery Mason-Blaug
// Date: 2022-06-13
//////////////////////////////////////////////////////////


#include "Engine/heightmap.hpp"
#include "Engine/asset_loader.hpp"

std::pair<utl::vector<static_vertex_t>, utl::vector<f32>>  heightmap_t::load_vertices(
                                                                                      asset_loader_t* loader_ptr, 
                                                                                      const std::string& heightmap_path, 
                                                                                      f32 max_height, f32 texel_width, f32 texel_height)
{
    auto& loader = *loader_ptr;
    auto texture = loader.get_texture2d(heightmap_path);
//    texture.get().mipmap();
    auto texture_accessor = texture.get().map_buffer();
    
    auto size = 4;
    const auto line_size = size*texture_accessor.width;
    
    utl::vector<static_vertex_t> indexed_verts;
    utl::vector<static_vertex_t> vertices;
    utl::vector<f32> heightmap;
    utl::vector<u32> indices;
    const v3f tangent = v3f(1,0,0);
    const v3f bitangent = v3f(0,0,1);
    
    const f32 map_width = texel_width   * static_cast<f32>(texture_accessor.width);
    const f32 map_height = texel_height * static_cast<f32>(texture_accessor.height);
    for(int y = 0; y < texture_accessor.height; y+=1){
        for(int x = 0; x < texture_accessor.width; x+=1){
            const f32 height = texture_accessor.data[(y*line_size+x*size)+0]/256.0f;
            static_vertex_t vert;
            
            f32 px = f32(x)/f32(texture_accessor.width);
            f32 py = f32(y)/f32(texture_accessor.height);
            px -= 0.5f;
            py -= 0.5f;
            vert.normal = v3f(0,1,0);
            vert.position = (tangent * px * 2.0f * map_width) + (bitangent * py * 2.0f * map_height);
            vert.position.y = max_height*height;
            
            vert.tex_coord = v2f(px+0.5f,py+0.5f);
            vert.tex_coord.y = 1.0f-vert.tex_coord.y;
            indexed_verts.push_back(vert);
            heightmap.push_back(vert.position.y);
            int id = (y) * texture_accessor.width + (x);
            if (x != texture_accessor.width - 1 && y != texture_accessor.height - 1)
            {
                indices.push_back(id);
                indices.push_back(id + texture_accessor.width + 1);
                indices.push_back(id + 1);
                
                indices.push_back(id);
                indices.push_back(id + texture_accessor.width);
                indices.push_back(id + texture_accessor.width + 1);
            }
        }
    }
    // calculate normals
    for (size_t i = 0; i < indexed_verts.size(); i++) {
        glm::vec3 v0 = indexed_verts[i].position;
		glm::vec3 v1;
		glm::vec3 v2;
        
		// all but last row
		// look "down" the array
		if (i < (indexed_verts.size() - (texture_accessor.width)))
		{
			v1 = indexed_verts[i + (texture_accessor.width)].position;
			if ((i % texture_accessor.width) != (texture_accessor.width - 1)) // is not the right edge
			{
				v2 = indexed_verts[i+1].position;
			}
			else // right edge, need to flip
			{
				v2 = v1; // swap so cross prod is correct
				v1 = indexed_verts[i-1].position;
			}
		}
		else // last row looking "up"
		{
			v2 = indexed_verts[i - texture_accessor.width].position;
			if ((i % texture_accessor.width) != texture_accessor.width - 1) // is not the right edge
			{
				v1 = indexed_verts[i + 1].position;
			}
			else // right edge, need to flip
			{
				v1 = indexed_verts[i - 1].position;
			}
		}
		indexed_verts[i].normal = -glm::normalize(glm::cross(v2 - v0, v1 - v0)); // RHR
    }
    
    // smooth vertices
    const auto w = texture_accessor.width;
    for (size_t smooth_count = 0; smooth_count <1; smooth_count++) {
        for (size_t y = w; y < indexed_verts.size() - w; y += w) {
            for (size_t x = 1; x < w?w-1:0; x++) {
                auto& v0 = indexed_verts[x+y];
                const auto& v1 = indexed_verts[x+y+1];
                const auto& v2 = indexed_verts[x+y-1];
                const auto& v3 = indexed_verts[x+y+w-1];
                const auto& v4 = indexed_verts[x+y+w+1];
                const auto& v5 = indexed_verts[x+y+w];
                const auto& v6 = indexed_verts[x+y-w+1];
                const auto& v7 = indexed_verts[x+y-w-1];
                const auto& v8 = indexed_verts[x+y-w];
                
                v0.position.y = ((v0.position.y+
                                v1.position.y+
                                v2.position.y+
                                v3.position.y+
                                v4.position.y+
                                v5.position.y+
                                v6.position.y+
                                v7.position.y+
                                v8.position.y)/9.0f);
                heightmap[x+y] = v0.position.y;
                
                v0.normal = glm::normalize((v0.normal+
                                            v1.normal+
                                            v2.normal+
                                            v3.normal+
                                            v4.normal+
                                            v5.normal+
                                            v6.normal+
                                            v7.normal+
                                            v8.normal)/9.0f);
            }
        }
    } 
    
    std::transform(indices.begin(), 
                   indices.end(), 
                   std::back_inserter(vertices),
                   [&](auto i) {return indexed_verts[i];});
    return {vertices, heightmap };
}