// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////

#include "core.hpp"

#include "Graphics/texture2d.hpp"
#include "Util/exceptions.hpp"
#include "Util/logger.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image.h"

#include <fmt/core.h>
#include <fmt/color.h>


namespace internal {
  decltype(texture2d_t::id) bound_texture = -1;  
};


void texture2d_t::mipmap() {
	//bind();
	glGenerateTextureMipmap(id);
}
void texture2d_t::bind_image() {

	glBindImageTexture(slot, id, 0, GL_FALSE, 0, GL_READ_WRITE, internal_format);
}

void texture2d_t::bind() {
    //if (internal::bound_texture != id) {
		internal::bound_texture = id;
		texture_i::bind_slot(slot);
		//glBindTexture(GL_TEXTURE_2D, id);
	//}
}

void texture2d_t::unbind() {
	//if (internal::bound_texture != 0) {
		internal::bound_texture = 0;
		texture_i::bind_slot(0);
		//glBindTexture(GL_TEXTURE_2D, 0);
	//}
}
texture2d_accessor_t::texture2d_accessor_t(GLuint id, int w, int h, GLenum f, GLuint mip_level)
: width(w >> mip_level), height(h >> mip_level), format(f)
{
	glGenBuffers(1, &pbo);
	glBindBuffer(GL_PIXEL_PACK_BUFFER, pbo);
	glBufferData(GL_PIXEL_PACK_BUFFER, width*height*4, nullptr, GL_STATIC_READ);

	glBindTexture(GL_TEXTURE_2D, id);
	glGetTexImage(GL_TEXTURE_2D, mip_level, GL_RGBA, GL_UNSIGNED_BYTE, (void*)0);
	data = (GLubyte*)glMapBuffer(GL_PIXEL_PACK_BUFFER, GL_READ_ONLY);
}

texture2d_accessor_t texture2d_t::map_buffer() {
	return texture2d_accessor_t(id, width, height, GL_RGBA, 0);
}
void texture2d_t::destroy() {
    glDeleteTextures(1, &id);
    id = 0;
}
void texture2d_t::set(const std::function<u32(int, int)>& fn) {
	std::vector<u32> data(width*height, 0x00);
	for(int y = 0; y < height; y++) {
		for(int x = 0; x < width; x++){
			data[y * width + x] = (fn(x,y));
		}
	}
	set_data(data);
}
void texture2d_t::set_data(const std::vector<u32>& data) {
	//const uint32_t bpp = data_format == GL_RGBA ? 4 : 3;
	if (data.size() != width * height) {
		throw runtime_error_x("Texture2d::set_data: Data doesn't match texture dimensions");
	}

	//bind();
	glTextureSubImage2D(id, 0, 0, 0, width, height, data_format, data_type, data.data());

}

void texture2d_t::set_wrap(bool wrap) {
	//bind();
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP_TO_EDGE);
}
void texture2d_t::set_filter(bool linear) {
	//bind();
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, linear ? GL_LINEAR : GL_NEAREST);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, linear ? GL_LINEAR : GL_NEAREST);
}

texture2d_t::texture2d_t(const std::string& path, const std::string& asset_dir) {
    internal_format = GL_RGBA8;
	data_format = GL_RGBA;
    data_type = GL_UNSIGNED_BYTE;
    
	stbi_set_flip_vertically_on_load(true);
    
	glCreateTextures(GL_TEXTURE_2D, 1, &id);
	//glGenTextures(1, &id);
	//glBindTexture(GL_TEXTURE_2D, id);
	float anisotropy = 16.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
	glTextureParameterf(id, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTextureParameteri(id, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTextureParameteri(id, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(id, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate the texture
    
	unsigned char* data = stbi_load(fmt::format("{}{}", asset_dir, path).c_str(), &width, &height, &channels, 0);
	if (data)
	{
		if (channels == 3) {
			glTextureStorage2D(id, 1, GL_RGB8, width, height);
			glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGB, data_type, data);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, data_type, data);
			data_format = GL_RGB;
		}
		else if (channels == 4) {
			glTextureStorage2D(id, 1, GL_RGBA8, width, height);
			glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGBA, data_type, data);
			//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, data_type, data);
		}
		glGenerateTextureMipmap(id);
		logger_t::info(fmt::format("Texture loaded: {}, {} x {} - channels: {}", path, width, height, channels));
	}
	else
	{
        throw runtime_error_x(fmt::format("Texture failed to load: {}", path));

		data = stbi_load(fmt::format("{}{}", asset_dir, "textures/UVgrid.png").c_str(), &width, &height, &channels, 0);
		if (data)
        {
            glTextureStorage2D(id, 0, GL_RGB, width, height);
			glTextureSubImage2D(id, 0, 0, 0, width, height, GL_RGB, data_type, data);
            glGenerateTextureMipmap(id);
        }
		logger_t::warn(fmt::format("Texture failed to load: {}", path));
	}
	stbi_image_free(data);
}

texture2d_t::texture2d_t(u32 w, u32 h) 
	: width(w), height(h)
{
    internal_format = GL_RGBA8;
	data_format = GL_RGBA;
    data_type = GL_UNSIGNED_BYTE;

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	float anisotropy = 16.f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, anisotropy);
    
	//glCreateTextures(GL_TEXTURE_2D, 1, &m_Texture);
	glTexStorage2D(GL_TEXTURE_2D, 1, internal_format, width, height);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, data_format, data_type, nullptr);
}