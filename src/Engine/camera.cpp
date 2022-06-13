// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#include "Engine/camera.hpp"

#include "glm/gtc/matrix_transform.hpp"

camera_t::camera_t(f32 f, f32 w, f32 h, f32 pn, f32 fa)
    : fov(f), width(w), height(h), n(pn), f(fa)
{
	projection = glm::perspectiveFov(fov, width, height, n, f);
}

void camera_t::look_at(const v3f& target, const v3f& up)
{
	view = glm::lookAt(position, target, up);
}




void camera_t::make_projection()
{
	projection = glm::perspectiveFov(fov, width, height, n, f);
}