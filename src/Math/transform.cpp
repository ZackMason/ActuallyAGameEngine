// Author: Zackery Mason-Blaug
// Date: 2022-06-06
//////////////////////////////////////////////////////////


#include "Math/transform.hpp"

#include "Util/exceptions.hpp"
#include <glm/gtx/quaternion.hpp>

m44 transform_t::to_matrix() const
{
	m44 res = basis;
	res[3] = v4f(origin,1.0);
	return res;
}

void transform_t::translate(const v3f& delta)
{
	origin += delta;
}

void transform_t::scale(const v3f& delta)
{
	basis = glm::scale(m44(basis), delta);
}

void transform_t::rotate(const v3f& delta)
{
	m44 rot = m44(1.0f);
	rot = glm::rotate(rot, delta.z, { 0,0,1 });
	rot = glm::rotate(rot, delta.y, { 0,1,0 });
	rot = glm::rotate(rot, delta.x, { 1,0,0 });
	basis = (basis) * m33(rot);
}


void transform_t::rotate_quat(const glm::quat& delta)
{
	set_rotation(glm::quat(basis) * delta);
}

glm::quat transform_t::get_orientation() const {
	return glm::quat_cast(basis);
}

v3f transform_t::get_origin() const
{
	return origin;
}

transform_t transform_t::inverse() const
{
	transform_t transform;
	transform.basis = glm::transpose(basis);
	transform.origin = basis * -origin;
	return transform;
}

void transform_t::set_fov(f32 fov, f32 width, f32 height, f32 near, f32 far)
{
	auto mat = glm::perspectiveFov(fov, width, height, near, far);
	basis = mat;
	origin = mat[3];
}

void transform_t::look_at(const v3f& target, const v3f& up)
{
	auto mat = glm::lookAt(origin, target, up);
	basis = mat;
	origin = mat[3];
}

void transform_t::set_position(const v3f& position)
{
	origin = position;
}

void transform_t::set_scale(const v3f& scale)
{
	for (int i = 0; i < 3; i++)
		basis[i] = glm::normalize(basis[i]) * scale[i];
}

void transform_t::set_rotation(const v3f& rotation)
{
    auto nbasis = m44{1.0f};
    nbasis = glm::rotate(nbasis, rotation.x, v3f{1,0,0});
    nbasis = glm::rotate(nbasis, rotation.y, v3f{0,1,0});
    nbasis = glm::rotate(nbasis, rotation.z, v3f{0,0,1});
    basis = nbasis;
    //throw not_implemented_x();
}

void transform_t::set_rotation(const glm::quat& quat)
{
	basis = glm::toMat3(glm::normalize(quat));
}
