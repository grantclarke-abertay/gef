#ifndef _GEF_MATRIX_44_H
#define _GEF_MATRIX_44_H

#include <gef.h>
#include <stdlib.h>
#include <maths/vector4.h>

namespace gef
{
	class Vector3;
	class Quaternion;


	class Matrix44
	{
	public:
		void SetIdentity();
		void SetZero();
		void LookAt(const gef::Vector3& eye, const gef::Vector3& lookat, const gef::Vector3& up);
		void PerspectiveFrustumGL(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);
		void PerspectiveFovGL(const float fov, const float aspect_ratio, const float near_dist, const float far_dist);
		void OrthographicFrustumGL(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);
		void PerspectiveFrustumD3D(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);
		void PerspectiveFovD3D(const float fov, const float aspect_ratio, const float near_dist, const float far_dist);
		void OrthographicFrustumLHD3D(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);
		void OrthographicFrustumRHD3D(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);
		inline void PerspectiveFrustumVita(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist)
		{
			PerspectiveFrustumGL(left, right, top, bottom, near_dist, far_dist);
		}
	
		inline void PerspectiveFovVita(const float fov, const float aspect_ratio, const float near_dist, const float far_dist)
		{
			PerspectiveFovGL(fov, aspect_ratio, near_dist, far_dist);
		}
	
		inline void OrthographicFrustumVita(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist)
		{
			OrthographicFrustumGL(left, right, top, bottom, near_dist, far_dist);
		}

		inline void PerspectiveFrustumPS4(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist)
		{
			PerspectiveFrustumGL(left, right, top, bottom, near_dist, far_dist);
		}
	
		inline void PerspectiveFovPS4(const float fov, const float aspect_ratio, const float near_dist, const float far_dist)
		{
			PerspectiveFovGL(fov, aspect_ratio, near_dist, far_dist);
		}
	
		inline void OrthographicFrustumPS4(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist)
		{
			OrthographicFrustumGL(left, right, top, bottom, near_dist, far_dist);
		}
	
		void RotationX(float radiansRot);
		void RotationY(float radiansRot);
		void RotationZ(float radiansRot);
		void Rotation(const gef::Quaternion& quat);
		void Scale(const gef::Vector3& scale);
		Vector3 GetScale() const;
		void SetTranslation(const gef::Vector3& trans);
		const Vector3 GetTranslation() const;
		void Transpose(const Matrix44& matrix);
		void AffineInverse(const Matrix44& matrix);
		void NormaliseRotation();
		float CalculateDeterminant() const;
		void Inverse(const Matrix44 matrix, float* determinant = NULL);


		const Matrix44 operator*(const Matrix44& matrix) const;

		inline const Vector4& GetRow(int row) const
		{
			return values_[row];
		}

		inline Vector4 GetColumn(int column) const
		{
			return Vector4(values_[0][column], values_[1][column], values_[2][column], values_[3][column]);
		}

		inline void SetRow(int row, const Vector4& row_values)
		{
			values_[row] = row_values;
		}

		inline void SetColumn(int column, const Vector4& column_values)
		{
			*(((float*)&values_[0]) + column) = column_values.x();
			*(((float*)&values_[1]) + column) = column_values.y();
			*(((float*)&values_[2]) + column) = column_values.z();
			*(((float*)&values_[3]) + column) = column_values.w();
		}

		inline float m(int row, int column) const
		{
			return *(((float*)&values_[row]) + column);
		}

		inline void set_m(int row, int column, float value)
		{
			*(((float*)&values_[row]) + column) = value;
		}

	protected:
//		float m[4][4];
		Vector4 values_[4];
	};
}

#endif // _GEF_MATRIX_44_H
