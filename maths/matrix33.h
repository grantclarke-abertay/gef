#ifndef _GEF_MATRIX_33_H
#define _GEF_MATRIX_33_H

#include <gef.h>

namespace gef
{
	class Vector4;
	class Quaternion;
	class Vector4;

	class Matrix33
	{
	public:

		float CalculateDeterminant() const;
		void Transpose(const Matrix33& matrix);
		void Inverse(const Matrix33 matrix, float& determinant);
		void Cofactor(const Matrix33 matrix);
		void OrthoNormalise(const Matrix33& matrix);

		const Matrix33 operator*(const Matrix33& matrix) const;
		const Matrix33 operator*(const float scalar) const;

		static const Matrix33 kIdentity;
		static const Matrix33 kZero;

		inline void SetIdentity() { *this = Matrix33::kIdentity; }
		inline void SetZero() { *this = Matrix33::kZero; }
		inline void SetColumn(const Int32 column, const float x, const float y, const float z) { m[0][column] = x; m[1][column] = y; m[2][column] = z;} 
		inline void SetRow(const Int32 row, const float x, const float y, const float z) { m[row][0] = x; m[row][1] = y; m[row][2] = z;} 

		float m[3][3];
	};
}

#endif // gef_MATRIX_33_H

