// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Math/ray.hpp"

struct camera_t {

    camera_t(f32 f, f32 w, f32 h, f32 n, f32 fa);
    
	void look_at(const v3f& pos, const v3f& up = { 0,1,0 });
    
	m44 view_projection() const
	{
		return projection * view;
	}
    
	void make_projection();
    
	auto forward() const {
		return -glm::normalize(v3f(glm::transpose(view)[2])); 
	}
	auto right() const {
		return glm::normalize(v3f(glm::transpose(view)[0]));  
	}
    auto up() const {
		return -glm::normalize(v3f(glm::transpose(view)[1]));
	}
    auto back() const {
		return -forward();
	}
	auto left() const {
		return -right();
	}
    auto down() const {
		return -up();
	}
    
	virtual v3f get_position() const {
		return position;
	}
    
	ray_t get_ray(f32 x, f32 y)	{
		const v4f ndc = {
			(2.0f * x) / width - 1.0f,
			1.0f - (2.0f * y) / height,
			-1.0f, 1.0f,
		};
        
		v4f ray_eye = glm::inverse(projection) * ndc;
		ray_eye.z = -1.0f;
		ray_eye.w = 0.0f;
		return { position, glm::normalize(v3f(glm::inverse(view) * ray_eye)) };
	}

    f32 fov = 45.0f;
	f32 width = 100.0f;
	f32 height = 100.0f; 
	f32 n = 0.01f;
	f32 f = 1000.f;
    
	v3f position = { 0,0,0 };
	m44 view;
	m44 projection;
};

