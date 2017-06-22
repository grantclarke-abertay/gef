#ifndef _GEF_VECTOR3_H
#define _GEF_VECTOR3_H


namespace gef
{
class Vector4
{
public:

	Vector4();
	~Vector4();
	Vector4(const float new_x, const float new_y, const float new_z);
	Vector4(const float new_x, const float new_y, const float new_z, const float new_w);

	const Vector4 operator - (const Vector4& _vec) const;
	const Vector4 operator + (const Vector4& _vec) const;
	Vector4& operator -= (const Vector4& _vec);
	Vector4& operator += (const Vector4& _vec);
	const Vector4 operator * (const float _scalar) const;
	const Vector4 operator / (const float _scalar) const;
	Vector4& operator *= (const float _scalar);
	Vector4& operator /= (const float _scalar);
	const float operator[] (const int index) const;

	void Normalise();
	float LengthSqr() const;
	float Length() const;
	float DotProduct(const Vector4& _vec) const;
	const Vector4 CrossProduct(const Vector4& _vec) const;
	const Vector4 Transform(const class Matrix44& _mat) const;
	const Vector4 TransformNoTranslation(const class Matrix44& _mat) const;
	const Vector4 Transform(const class Matrix33& _mat) const;
	const Vector4 TransformCM(const class Matrix33& _mat) const;
	void Lerp(const Vector4& start, const Vector4& end, const float time);

	const Vector4 TransformW(const class Matrix44& _mat) const;
	const Vector4 CrossProduct3(const Vector4& v2, const Vector4& v3) const;


	float x() const;
	float y() const;
	float z() const;
	float w() const;
	void set_x(float x);
	void set_y(float y);
	void set_z(float z);
	void set_w(float w);
	void set_value(float x, float y, float z);
	void set_value(float x, float y, float z, float w);
protected:
	// store values as an array of floats so the implementation
	// can be changed at a later date to support use SIMD instructions
	float values_[4];
public:
	static const Vector4 kZero;
	static const Vector4 kOne;
};

}

#include "maths/vector4.inl"

#endif // _GEF_VECTOR3_H
