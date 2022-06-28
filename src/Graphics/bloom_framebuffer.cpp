// Author: Zackery Mason-Blaug
// Date: 2022-06-27
//////////////////////////////////////////////////////////


#include "Graphics/bloom_framebuffer.hpp"

#include "Util/exceptions.hpp"

#include "glad/glad.h"

void bloom_framebuffer_t::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
}

bloom_framebuffer_t::bloom_framebuffer_t(u32 w, u32 h, u32 mip_length) {
    width = w;
    height = h;
    glGenFramebuffers(1, &id);
    glBindFramebuffer(GL_FRAMEBUFFER, id);

    v2f fmip_size{f32(w), f32(h)};
    v2i mip_size{int(w), int(h)};

    for (unsigned int i = 0; i < mip_length; i++)
    {
        bloom_mip_t mip;

        fmip_size *= 0.5f;
        mip_size /= 2;
        mip.fsize = fmip_size;
        mip.size = mip_size;

        glGenTextures(1, &mip.texture);
        glBindTexture(GL_TEXTURE_2D, mip.texture);
        // we are downscaling an HDR color buffer, so we need a float texture format
        glTexImage2D(GL_TEXTURE_2D, 0, GL_R11F_G11F_B10F,
                     (int)fmip_size.x, (int)fmip_size.y,
                     0, GL_RGB, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        mip_chain.emplace_back(mip);
    }

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, mip_chain[0].texture, 0);

    // setup attachments
    unsigned int attachments[1] = { GL_COLOR_ATTACHMENT0 };
    glDrawBuffers(1, attachments);

    // check completion status
    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        throw runtime_error_x("Failed to build bloom framebuffer");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
bloom_framebuffer_t::~bloom_framebuffer_t() {
    for (int i = 0; i < mip_chain.size(); i++) {
        glDeleteTextures(1, &mip_chain[i].texture);
        mip_chain[i].texture = 0;
    }
    glDeleteFramebuffers(1, &id);
    id = 0;
}

void bloom_framebuffer_t::resize(int w, int h)
{
	if (w == width && h == height) return;
	
	new (this) bloom_framebuffer_t(w, h, mip_chain.size());
}