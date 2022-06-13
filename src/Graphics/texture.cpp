// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/texture.hpp"
#include <stdexcept>

void texture_i::bind_slot(GLuint new_slot) {
    slot = new_slot;
    if (slot < 0 || slot >= 32) {
        throw std::runtime_error("Invalid texture slot");
    }
    glActiveTexture(GL_TEXTURE0 + slot);
}