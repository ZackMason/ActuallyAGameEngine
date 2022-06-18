// Author: Zackery Mason-Blaug
// Date: 2022-06-08
//////////////////////////////////////////////////////////


#include "Graphics/framebuffer.hpp"


#include <fmt/core.h>
#include <fmt/color.h>

#include "Util/exceptions.hpp"

// TODO: Implement logarithmic depth buffer

framebuffer_t::framebuffer_t(int w, int h, bool aa)
: width(w), height(h), msaa(aa)
{
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
	glGenTextures(1, &tbo);
	
	if (msaa)
	{
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tbo);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, width, height, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, tbo);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, NULL);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	if (msaa)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, tbo, 0);
	}
	else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tbo, 0);
    
    
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	if(msaa)
		glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT32F, width, height);
	else
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT32F, width, height);
    
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);
    
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		throw runtime_error_x("Framebuffer: failed to create framebuffer");
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void framebuffer_t::blit(framebuffer_t& msaa_fbo)
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, msaa_fbo.fbo);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
	glBlitFramebuffer(0, 0, msaa_fbo.width, msaa_fbo.height, 0, 0, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

framebuffer_t::~framebuffer_t()
{
	glDeleteFramebuffers(1, &fbo);
	glDeleteRenderbuffers(1, &rbo);
	glDeleteTextures(1, &tbo);
}


void framebuffer_t::draw()
{

}
void framebuffer_t::bind()
{
	switch (mode)
	{
        case target_e::RENDER_TARGET:
		    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    		break;
        case target_e::TEXTURE:
		    assert(slot >= 0 && slot < 32);
		    glActiveTexture(GL_TEXTURE0 + slot);
		    glBindTexture(msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, tbo);
		    break;
	}
}

void framebuffer_t::mipmap() {
	glGenerateMipmap(msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D);
}

void framebuffer_t::unbind()
{
	switch (mode)
	{
        case target_e::RENDER_TARGET:
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            break;
        case target_e::TEXTURE:
            glBindTexture(msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
            break;
	}
}

void framebuffer_t::clear(const v4f& color, GLenum flags)
{
	bind();

	switch (mode)
	{
        case target_e::RENDER_TARGET:
			glClearColor(color.r, color.g, color.b, color.a);
			glClear(flags);
            break;
        case target_e::TEXTURE:
			throw not_implemented_x();
            glBindTexture(msaa ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D, 0);
            break;
	}
}

void framebuffer_t::resize(int w, int h)
{
	if (w == width && h == height) return;
	
	new (this) framebuffer_t(w, h, msaa);

	//throw not_implemented_x();
}
