// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/texture3d.hpp"

#include "Util/exceptions.hpp"

#include <fmt/core.h>
#include <fmt/color.h>

void texture3d_t::bind() {
    texture_i::bind_slot(slot);
    glBindTexture(GL_TEXTURE_3D, id);
}

void texture3d_t::generate_mipmaps() {
    bind();
    glGenerateMipmap(GL_TEXTURE_3D);
    unbind();
}
void texture3d_t::unbind() {
    texture_i::bind_slot(slot);
    glBindTexture(GL_TEXTURE_3D, 0);
}
void texture3d_t::destroy() {
    glDeleteTextures(1, &id);
    id = 0;
}

texture3d_t::texture3d_t(u32 w, u32 h, u32 d) {
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_3D, id);
    
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexStorage3D(GL_TEXTURE_3D, 7, GL_RGBA8, width, height, depth);
    
    std::vector<GLubyte> data(width*height*depth*4, 0);
    glTexSubImage3D(GL_TEXTURE_3D, 0, 0, 0, 0, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data.data());
    glBindTexture(GL_TEXTURE_3D, 0);
}