// Author: Zackery Mason-Blaug
// Date: 2022-06-07
//////////////////////////////////////////////////////////


#pragma once

#include "core.hpp"

#include "Math/ray.hpp"
#include "Math/aabb.hpp"
#include "Math/transform.hpp"

#include "Util/vector.hpp"

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
    
	utl::vector<glm::vec4> get_frustum_corners() const
	{
		const auto inv = glm::inverse(view_projection());
		
		std::vector<glm::vec4> frustumCorners;
		for (unsigned int x = 0; x < 2; ++x)
		{
			for (unsigned int y = 0; y < 2; ++y)
			{
				for (unsigned int z = 0; z < 2; ++z)
				{
					const glm::vec4 pt = 
						inv * glm::vec4(
							2.0f * x - 1.0f,
							2.0f * y - 1.0f,
							2.0f * z - 1.0f,
							1.0f);
					frustumCorners.push_back(pt / pt.w);
				}
			}
		}
		
		return frustumCorners;
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
		return { get_position(), glm::normalize(v3f(glm::inverse(view) * ray_eye)) };
	}

	bool cull_aabb(
		const aabb_t<v3f>& aabb,
		const transform_t& tr
	) const {

		if (tr.xform_aabb(aabb).contains(position
		)) {
			return false;
		}

		const auto within = [](f32 a, f32 b, f32 c) {
			return a <= b && b <= c;
		};

		const v4f corners[8] = {
			{aabb.min.x, aabb.min.y, aabb.min.z, 1.0f},
			{aabb.max.x, aabb.min.y, aabb.min.z, 1.0f},
			{aabb.min.x, aabb.max.y, aabb.min.z, 1.0f},
			{aabb.max.x, aabb.max.y, aabb.min.z, 1.0f},

			{aabb.min.x, aabb.min.y, aabb.max.z, 1.0f},
			{aabb.max.x, aabb.min.y, aabb.max.z, 1.0f},
			{aabb.min.x, aabb.max.y, aabb.max.z, 1.0f},
			{aabb.max.x, aabb.max.y, aabb.max.z, 1.0f},
		};

		bool inside = false;
		for (size_t i = 0; i < 8; i++) {
			const v4f corner =  view_projection() * tr.to_matrix() * corners[i];
			inside = inside || within(-corner.w, corner.x, corner.w) &&
				within(-corner.w, corner.y, corner.w) &&
				within(0.0f, corner.z, corner.w);
		}
		return !inside;
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

