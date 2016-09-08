#ifndef _GEF_VECTOR4_H
#define _GEF_VECTOR4_H

#include <maths/vector3.h>

namespace gef
{

	// TODO: Get this class inheriting from Vector3
class Vector4 : public Vector3
{
public:
	Vector4();
	Vector4(const float x, const float y, const float z, const float w);

	const Vector4 Transform(const class Matrix44& _mat) const;
	const Vector4 CrossProduct(const Vector4& v2, const Vector4& v3) const;

	float w() const;
	void set_w(float w);
	void set_value(float x, float y, float z, float w);

	static const Vector4 kZero;
};

}

#include "vector4.inl"

#endif // _GEF_VECTOR4_H
