// Author: Zackery Mason-Blaug
// Date: 2022-06-29
//////////////////////////////////////////////////////////


#include "Graphics/cascade_shadowmap.hpp"

#include "glad/glad.h"

#include "Util/exceptions.hpp"

namespace internal {
    int size = 4096;
    GLint viewport[4];
};

void cascade_shadowmap_t::unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glCullFace(GL_BACK);
    glViewport(0, 0, internal::viewport[2], internal::viewport[3]);
}
void cascade_shadowmap_t::bind_depth_map(unsigned int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_maps_id);
}

void cascade_shadowmap_t::bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glGetIntegerv(GL_VIEWPORT, internal::viewport);
    glViewport(0, 0, internal::size, internal::size);
    glCullFace(GL_FRONT);
    glClear(GL_DEPTH_BUFFER_BIT);

}

cascade_shadowmap_t::cascade_shadowmap_t() {
    glGenFramebuffers(1, &fbo_id);
    glGenTextures(1, &depth_maps_id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depth_maps_id);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_DEPTH_COMPONENT32F, 
        internal::size, internal::size, 8+1 /* hard coded level size */, 
        0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    constexpr float border_color[] = {1.0f, 1.0f, 1.0f, 1.0f};
    glTexParameterfv(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_BORDER_COLOR, border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_maps_id, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    int status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        throw runtime_error_x(fmt::format("Failed to create CSM framebuffer"));
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

