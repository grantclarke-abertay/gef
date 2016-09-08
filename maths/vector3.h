#ifndef _GEF_VECTOR3_H
#define _GEF_VECTOR3_H


namespace gef
{
class Vector3
{
public:

	Vector3();
	~Vector3();
	Vector3(const float new_x, const float new_y, const float new_z);

	const Vector3 operator - (const Vector3& _vec) const;
	const Vector3 operator + (const Vector3& _vec) const;
	Vector3& operator -= (const Vector3& _vec);
	Vector3& operator += (const Vector3& _vec);
	const Vector3 operator * (const float _scalar) const;
	const Vector3 operator / (const float _scalar) const;
	Vector3& operator *= (const float _scalar);
	Vector3& operator /= (const float _scalar);
	const float operator[] (const int index) const;

	void Normalise();
	float LengthSqr() const;
	float Length() const;
	float DotProduct(const Vector3& _vec) const;
	const Vector3 CrossProduct(const Vector3& _vec) const;
	const Vector3 Transform(const class Matrix44& _mat) const;
	const Vector3 TransformNoTranslation(const class Matrix44& _mat) const;
	const Vector3 Transform(const class Matrix33& _mat) const;
	const Vector3 TransformCM(const class Matrix33& _mat) const;
	void Lerp(const Vector3& start, const Vector3& end, const float time);

	float x() const;
	float y() const;
	float z() const;
	void set_x(float x);
	void set_y(float y);
	void set_z(float z);
	void set_value(float x, float y, float z);
protected:
	// store values as an array of floats so the implementation
	// can be changed at a later date to support use SIMD instructions
	float values_[4];

	static const Vector3 kZero;
};

}

#include "maths/vector3.inl"

#endif // _GEF_VECTOR3_H
