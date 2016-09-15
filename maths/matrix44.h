#ifndef _GEF_MATRIX_44_H
#define _GEF_MATRIX_44_H

#include <gef.h>
#include <stdlib.h>
#include <maths/vector4.h>

namespace gef
{
	class Vector4;
	class Quaternion;

	/**
	A homogeneous 4x4 matrix
	*/
	class Matrix44
	{
	public:
		/// @brief Set this matrix to the identity matrix
		void SetIdentity();

		/// @brief Set all elements of the matrix to zero
		void SetZero();

		/// @brief Set this matrix to a view matrix given a virtual cameras' eye and look at positions.
		/// @param[in] eye		The position the virtual camera is located.
		/// @param[in] lookat	The position the virtual camera is aimed at.
		/// @param[in] up		The direction that defines the up axis of the virtual camera.
		void LookAt(const gef::Vector4& eye, const gef::Vector4& lookat, const gef::Vector4& up);

		/// @brief Set this matrix to an OpenGL style perspective projection matrix given clipping plane coordinates.
		/// @param[in] left			The left vertical clipping plane.
		/// @param[in] right		The right vertical clipping plane.
		/// @param[in] top			The top horizontal clipping plane.
		/// @param[in] bottom		The bottom horizontal clipping plane.
		/// @param[in] near_dist	The near depth clipping plane. Must be positive.
		/// @param[in] far_dist		The far depth clipping plane. Must be positive.
		/// @note Clipping box in OpenGL is defined with bound (-1, -1, -1) to (1, 1, 1)
		void PerspectiveFrustumGL(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);

		/// @brief Set this matrix to an OpenGL style perspective projection matrix given the field of view.
		/// @param[in] fov			The vertical field of view angle, in radians.
		/// @param[in] aspect_ratio	The aspect ratio that determines the horizontal field of view. The aspect ratio is the ratio of width to height.
		/// @param[in] near_dist	The near depth clipping plane. Must be positive.
		/// @param[in] far_dist		The far depth clipping plane. Must be positive.
		/// @note Clipping box in OpenGL is defined with bound (-1, -1, -1) to (1, 1, 1)
		void PerspectiveFovGL(const float fov, const float aspect_ratio, const float near_dist, const float far_dist);

		/// @brief Set this matrix to an OpenGL style orthographic projection matrix given clipping plane coordinates.
		/// @param[in] left			The left vertical clipping plane.
		/// @param[in] right		The right vertical clipping plane.
		/// @param[in] top			The top horizontal clipping plane.
		/// @param[in] bottom		The bottom horizontal clipping plane.
		/// @param[in] near_dist	The near depth clipping plane.
		/// @param[in] far_dist		The far depth clipping plane.
		/// @note Clipping box in OpenGL is defined with bound (-1, -1, -1) to (1, 1, 1)
		void OrthographicFrustumGL(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);

		/// @param[in] left			The left vertical clipping plane.
		/// @param[in] right		The right vertical clipping plane.
		/// @param[in] top			The top horizontal clipping plane.
		/// @param[in] bottom		The bottom horizontal clipping plane.
		/// @param[in] near_dist	The near depth clipping plane. Must be positive.
		/// @param[in] far_dist		The far depth clipping plane. Must be positive.
		/// @note Clipping box in Direct3D is defined with bound (-1, -1, 0) to (1, 1, 1)
		void PerspectiveFrustumD3D(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);

		/// @brief Set this matrix to an Direct3D style perspective projection matrix given the field of view.
		/// @param[in] fov			The vertical field of view angle, in radians.
		/// @param[in] aspect_ratio	The aspect ratio that determines the horizontal field of view. The aspect ratio is the ratio of width to height.
		/// @param[in] near_dist	The near depth clipping plane. Must be positive.
		/// @param[in] far_dist		The far depth clipping plane. Must be positive.
		/// @note Clipping box in Direct3D is defined with bound (-1, -1, 0) to (1, 1, 1)
		void PerspectiveFovD3D(const float fov, const float aspect_ratio, const float near_dist, const float far_dist);

		/// @brief Set this matrix to an Direct3D style orthographic projection matrix given clipping plane coordinates.
		/// @param[in] left			The left vertical clipping plane.
		/// @param[in] right		The right vertical clipping plane.
		/// @param[in] top			The top horizontal clipping plane.
		/// @param[in] bottom		The bottom horizontal clipping plane.
		/// @param[in] near_dist	The near depth clipping plane.
		/// @param[in] far_dist		The far depth clipping plane.
		/// @note Clipping box in Direct3D is defined with bound (-1, -1, 0) to (1, 1, 1)
		void OrthographicFrustumD3D(const float left, const float right, const float top, const float bottom, const float near_dist, const float far_dist);


		/// @brief Set this martix to a rotation matrix around the x-axis.
		/// @param[in] radiansRot	Angle of rotation.
		/// @note A positive angle of rotation results in a anti-clockwise rotation when looking towards the origin from the positive axis direction.
		/// @note This function overwrites all current elements of the matrix.
		void RotationX(float radiansRot);

		/// @brief Set this martix to a rotation matrix around the y-axis.
		/// @param[in] radiansRot	Angle of rotation.
		/// @note A positive angle of rotation results in a anti-clockwise rotation when looking towards the origin from the positive axis direction.
		/// @note This function overwrites all current elements of the matrix.
		void RotationY(float radiansRot);

		/// @brief Set this martix to a rotation matrix around the z-axis.
		/// @param[in] radiansRot	Angle of rotation.
		/// @note A positive angle of rotation results in a anti-clockwise rotation when looking towards the origin from the positive axis direction.
		/// @note This function overwrites all current elements of the matrix.
		void RotationZ(float radiansRot);

		/// @brief Set this matrix to the rotation matrix created from a quaternion.
		/// @param[in] quat		Rotation as a quaternion.
		/// @note This function overwrites all current elements of the matrix.
		void Rotation(const gef::Quaternion& quat);

		/// @brief Set this matrix to a scale matrix.
		/// @param[in] scale	Scale values for xyz axes.
		/// @note This function overwrites all current elements of the matrix.
		void Scale(const gef::Vector4& scale);

		/// @brief Get the Scale values for the xyz axes.
		/// @return The scale values for the xyz axes.
		Vector4 GetScale() const;

		/// @brief Set the elements in the matrix that represent the translation.
		/// @param[in] trans	The translation.
		/// @note This function only overwrites the elements where the translation is stored. All other elements remain unaltered.
		void SetTranslation(const gef::Vector4& trans);

		/// @brief Get the translation from this matrix.
		/// @return The translation.
		const Vector4 GetTranslation() const;

		/// @brief Set this matrix to the transpose of the matrix provided.
		/// @param[in] matrix	The matrix to be transposed.
		void Transpose(const Matrix44& matrix);

		/// @brief Set this matrix to the inverse of the matrix provided.
		/// @param[in] matrix	The affine transformation matrix to be inverted.
		/// @note It is assumed that the matrix passed in is an affine transformation matrix.
		void AffineInverse(const Matrix44& matrix);

		/// @brief Removes an scaling from the rotational component of this matrix.
		void NormaliseRotation();

		/// @brief Calculate the determinant of this matrix.
		/// @return the calculated determinant.
		float CalculateDeterminant() const;

		/// @brief Set this matrix to the inverse of the matrix provided.
		/// @param[in] matrix	The matrix to be inverted.
		/// @param[out] determinant		the determinant calculated to carry out the inverse operation. This can be set to NULL if it's not required.
		void Inverse(const Matrix44 matrix, float* determinant = NULL);


		/// @brief Calculate the product of two matrices.
		/// @param[in] matrix	The matrix for the second operand of the operation.
		/// @return The result of the operation.
		const Matrix44 operator*(const Matrix44& matrix) const;

		/// @brief Get a particular row from this matrix.
		/// @param[in] row		The row number.
		/// @return The contents of selected row.
		inline const Vector4& GetRow(int row) const
		{
			return values_[row];
		}

		/// @brief Get a particular column from this matrix.
		/// @param[in] column	The column number.
		/// @return The contents of selected column.
		inline Vector4 GetColumn(int column) const
		{
			return Vector4(values_[0][column], values_[1][column], values_[2][column], values_[3][column]);
		}

		/// @brief Set a particular row in this matrix with the values provided.
		/// @param[in] row			The row number.
		/// @param[in] row_values	The new row values.
		inline void SetRow(int row, const Vector4& row_values)
		{
			values_[row] = row_values;
		}

		/// @brief Set a particular column in this matrix with the values provided.
		/// @param[in] column			The column number.
		/// @param[in] column_values	The new row values.
		inline void SetColumn(int column, const Vector4& column_values)
		{
			*(((float*)&values_[0]) + column) = column_values.x();
			*(((float*)&values_[1]) + column) = column_values.y();
			*(((float*)&values_[2]) + column) = column_values.z();
			*(((float*)&values_[3]) + column) = column_values.w();
		}

		/// @brief Get the value of a particular element from this matrix.
		/// @param[in] row		The row number.
		/// @param[in] column	The column number.
		inline float m(int row, int column) const
		{
			return *(((float*)&values_[row]) + column);
		}

		/// @brief Set a particular element in this matrix to a the value provided.
		/// @param[in] row		The row number.
		/// @param[in] column	The column number.
		/// @param[in] value	The new value.
		inline void set_m(int row, int column, float value)
		{
			*(((float*)&values_[row]) + column) = value;
		}

	protected:
		/// The matrix is stored as 4 rows of Vectors
		Vector4 values_[4];
	};
}

#endif // _GEF_MATRIX_44_H
