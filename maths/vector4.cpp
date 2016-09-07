#include <maths/vector4.h>
#include <maths/matrix44.h>

namespace gef
{
	const Vector4 Vector4::kZero(0.0f, 0.0f, 0.0f, 0.0f);

	const Vector4 Vector4::Transform(const class Matrix44& _mat) const
	{
		Vector4 result;

		result.set_x(values_[0]*_mat.m(0,0)+values_[1]*_mat.m(1,0)+values_[2]*_mat.m(2,0)+values_[3]*_mat.m(3,0));
		result.set_y(values_[0]*_mat.m(0,1)+values_[1]*_mat.m(1,1)+values_[2]*_mat.m(2,1)+values_[3]*_mat.m(3,1));
		result.set_z(values_[0]*_mat.m(0,2)+values_[1]*_mat.m(1,2)+values_[2]*_mat.m(2,2)+values_[3]*_mat.m(3,2));
		result.set_w(values_[0]*_mat.m(0,3)+values_[1]*_mat.m(1,3)+values_[2]*_mat.m(2,3)+values_[3]*_mat.m(3,3));

		return result;
	}

	const Vector4 Vector4::CrossProduct(const Vector4& v1, const Vector4& v2) const
	{
		Vector4 result;
		result.set_x(  values_[1] * (v1.z() * v2.w() - v2.z() * v1.w()) - values_[2] * (v1.y() * v2.w() - v2.y() * v1.w()) + values_[3] * (v1.y() * v2.z() - v1.z() *v2.y()));
		result.set_y(-(values_[0] * (v1.z() * v2.w() - v2.z() * v1.w()) - values_[2] * (v1.x() * v2.w() - v2.x() * v1.w()) + values_[3] * (v1.x() * v2.z() - v2.x() * v1.z())));
		result.set_z(  values_[0] * (v1.y() * v2.w() - v2.y() * v1.w()) - values_[1] * (v1.x() *v2.w() - v2.x() * v1.w()) +  values_[3] * (v1.x() * v2.y() - v2.x() * v1.y()));
		result.set_w(-(values_[0] * (v1.y() * v2.z() - v2.y() * v1.z()) - values_[1] * (v1.x() * v2.z() - v2.x() *v1.z()) +  values_[2] * (v1.x() * v2.y() - v2.x() * v1.y())));
		return result;
	}

}