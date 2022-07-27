// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#pragma once

#include "types.hpp"

#include "Math/aabb.hpp"
#include "Math/ray.hpp"

struct transform_t {
    transform_t() = default;
	explicit transform_t(const m44& mat) : basis(mat), origin(mat[3]) {};
	transform_t(const v3f& position, const v3f& scale = {1,1,1}, const v3f& rotation = {0,0,0})
	: basis(m33(1.0f))
	{
		set_position(position);
		set_rotation(rotation);
		set_scale(scale);
	}
    
	m44 to_matrix() const;
    
	operator m44() const {
		return to_matrix();
	}
    
	void translate(const v3f& delta);
	void scale(const v3f& delta);
	void rotate(const v3f& delta);
	void rotate_quat(const glm::quat& delta);
    
	v3f get_origin() const;
	glm::quat get_orientation() const;
    
	transform_t inverse() const;
    
	void set_fov(f32 fov, f32 width, f32 height, f32 near, f32 far);
	void look_at(const v3f& target, const v3f& up = { 0,1,0 });
    
	void set_position(const v3f& position);
	void set_scale(const v3f& scale);
	void set_rotation(const v3f& rotation);
	void set_rotation(const glm::quat& quat);
	// in radians
	v3f get_euler_rotation() const;

	void normalize();

	void affine_invert() {
		basis = glm::inverse(basis);
		origin = basis * -origin;
	}

	v3f inv_xform(const v3f& vector) const
	{
		const v3f v = vector - origin;
		return glm::transpose(basis) * v;
	}

	v3f xform(const v3f& vector)const
	{
		return v3f(basis * vector) + origin;
	}
    
	ray_t xform(const ray_t& ray) const { 
		return ray_t{
			xform(ray.origin),
			glm::normalize(basis * ray.direction)
		};
	}

	aabb_t<v3f> xform_aabb(const aabb_t<v3f>& box)const
	{
		aabb_t<v3f> t_box;
		t_box.min = t_box.max = origin;
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
        {
            auto a = basis[j][i] * box.min[j];
            auto b = basis[j][i] * box.max[j];
            t_box.min[i] += a < b ? a : b;
            t_box.max[i] += a < b ? b : a;
        }
        
		//t_box.min = xform(box.min);
		//t_box.max = xform(box.max);
		return t_box;
	}
    
	m33 basis = m33(1.0f);
	v3f origin = v3f(0, 0, 0);
};

