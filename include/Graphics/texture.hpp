// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "Graphics/bindable.hpp"

struct texture_i : public bindable_i {
    void bind_slot(GLuint new_slot);

    GLuint slot{0};
};

