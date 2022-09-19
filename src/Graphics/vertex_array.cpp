// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Graphics/vertex_array.hpp"


namespace internal{
    GLint bound_array = {-1};
};

void vertex_array_t::reset() {
    internal::bound_array = -1;
}

void vertex_array_t::unbind()  {
    if (internal::bound_array != 0){
        internal::bound_array = 0;
        glBindVertexArray(0);
    }
}
void vertex_array_t::bind()  {
    if (internal::bound_array != id){
        internal::bound_array = id;
        glBindVertexArray(id);
    }
}


