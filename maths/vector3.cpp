#include <maths/vector3.h>
#include <maths/matrix44.h>
#include <maths/matrix33.h>
#include <maths/math_utils.h>
#include <math.h>

namespace gef
{
	const Vector3 Vector3::kZero(0.0f, 0.0f, 0.0f);

	float Vector3::LengthSqr() const
	{
		return values_[0]*values_[0] + values_[1]*values_[1] + values_[2]*values_[2];
	}

	float Vector3::Length() const
	{
		return sqrtf(LengthSqr());
	}

	void Vector3::Normalise()
	{
		float length = Length();
		values_[0] = values_[0] / length;
		values_[1] = values_[1] / length;
		values_[2] = values_[2] / length;
	}

	float Vector3::DotProduct(const Vector3& _vec) const
	{
		return values_[0]*_vec.x() + values_[1]*_vec.y() + values_[2]*_vec.z();
	}

	const Vector3 Vector3::CrossProduct(const Vector3& _vec) const
	{
		Vector3 result;

		result.set_x(values_[1]*_vec.z() - values_[2]*_vec.y());
		result.set_y(values_[2]*_vec.x() - values_[0]*_vec.z());
		result.set_z(values_[0]*_vec.y() - values_[1]*_vec.x());

		return result;
	}

	const Vector3 Vector3::Transform(const class Matrix44& _mat) const
	{
		Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

		result.set_x(values_[0]*_mat.m(0,0)+values_[1]*_mat.m(1,0)+values_[2]*_mat.m(2,0)+_mat.m(3,0));
		result.set_y(values_[0]*_mat.m(0,1)+values_[1]*_mat.m(1,1)+values_[2]*_mat.m(2,1)+_mat.m(3,1));
		result.set_z(values_[0]*_mat.m(0,2)+values_[1]*_mat.m(1,2)+values_[2]*_mat.m(2,2)+_mat.m(3,2));

		return result;
	}

	const Vector3 Vector3::Transform(const class Matrix33& _mat) const
	{
		Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

		result.set_x(values_[0]*_mat.m[0][0]+values_[1]*_mat.m[1][0]+values_[2]*_mat.m[2][0]);
		result.set_y(values_[0]*_mat.m[0][1]+values_[1]*_mat.m[1][1]+values_[2]*_mat.m[2][1]);
		result.set_z(values_[0]*_mat.m[0][2]+values_[1]*_mat.m[1][2]+values_[2]*_mat.m[2][2]);

		return result;
	}

	const Vector3 Vector3::TransformCM(const class Matrix33& _mat) const
	{
		Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

		result.set_x(values_[0]*_mat.m[0][0]+values_[1]*_mat.m[0][1]+values_[2]*_mat.m[0][2]);
		result.set_y(values_[0]*_mat.m[1][0]+values_[1]*_mat.m[1][1]+values_[2]*_mat.m[1][2]);
		result.set_z(values_[0]*_mat.m[2][0]+values_[1]*_mat.m[2][1]+values_[2]*_mat.m[2][2]);

		return result;
	}

	const Vector3 Vector3::TransformNoTranslation(const class Matrix44& _mat) const
	{
		Vector3 result = Vector3(0.0f, 0.0f, 0.0f);

		result.set_x(values_[0]*_mat.m(0,0)+values_[1]*_mat.m(1,0)+values_[2]*_mat.m(2,0));
		result.set_y(values_[0]*_mat.m(0,1)+values_[1]*_mat.m(1,1)+values_[2]*_mat.m(2,1));
		result.set_z(values_[0]*_mat.m(0,2)+values_[1]*_mat.m(1,2)+values_[2]*_mat.m(2,2));

		return result;
	}

	void Vector3::Lerp(const Vector3& start, const Vector3& end, const float time)
	{
		values_[0] = gef::Lerp(start.x(), end.x(), time);
		values_[1] = gef::Lerp(start.y(), end.y(), time);
		values_[2] = gef::Lerp(start.z(), end.z(), time);
	}
}