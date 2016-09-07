#ifndef _GEF_VECTOR4_H
#define _GEF_VECTOR4_H

namespace gef
{

	// TODO: Get this class inheriting from Vector3
class Vector4
{
public:
	Vector4();
	Vector4(const float x, const float y, const float z, const float w = 1.0f);

	const Vector4 Transform(const class Matrix44& _mat) const;
	const Vector4 CrossProduct(const Vector4& v2, const Vector4& v3) const;

	float x() const;
	float y() const;
	float z() const;
	float w() const;
	void set_x(float x);
	void set_y(float y);
	void set_z(float z);
	void set_w(float w);

	const float operator[] (const int index) const;

	static const Vector4 kZero;

protected:
	// store values as an array of floats so the implementation
	// can be changed at a later date to support use SIMD instructions
	float values_[4];

};

}

#include "vector4.inl"

#endif // _GEF_VECTOR4_H
