// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/bindable.hpp"
#include "types.hpp"

struct framebuffer_t : drawable_i {
	int width, height;
	bool msaa;
	GLuint tbo;
	GLuint rbo;
	GLuint fbo;
	GLuint slot;

    enum class target_e {
        RENDER_TARGET, TEXTURE, SIZE
    } mode = target_e::RENDER_TARGET;

	void mipmap();
	void blit(framebuffer_t& msaa_fbo);
	void resize(int w, int h);
	void clear(const v4f& color = { 0.0, 0.0, 0.0, 0.0 }, GLenum flags = GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    void draw() override;
    void bind() override;
    void unbind() override;

	framebuffer_t(int width, int height, bool _msaa = false);
	virtual ~framebuffer_t();
};

