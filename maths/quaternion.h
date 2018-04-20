#ifndef _GEF_QUATERNION_H
#define _GEF_QUATERNION_H

#include <math.h>
#include <maths/vector4.h>

namespace gef
{
	class Matrix44;

class Quaternion
{
public:
	Quaternion();
	Quaternion(float x, float y, float z, float w);
	Quaternion(const Matrix44& matrix);
	void SetFromMatrix(const class Matrix44& matrix);
	const Quaternion operator * (const Quaternion& quaternion) const;
	const Quaternion operator -() const;
	const Quaternion operator *(float scale) const;
	const Quaternion operator /(float scale) const;
	const Quaternion operator +(const Quaternion &q) const;
	float LengthSquared() const;
	float Length() const;
	void Normalise();
	void Identity();
	void Lerp(const Quaternion& startQ, const Quaternion& endQ, float time);
	void Slerp(const Quaternion& startQ, const Quaternion& endQ, float time);
	void Conjugate(const Quaternion& quaternion);

	static gef::Vector4 Rotate(const Quaternion& rotation, const Vector4& v);

	

	float x;
	float y;
	float z;
	float w;

	static const Quaternion kIdentity;
};

}

#include "quaternion.inl"


#endif // _GEF_QUATERNION_H
