#include <maths/matrix33.h>

#include <maths/matrix44.h>
#include <maths/vector4.h>
#include <maths/vector4.h>
#include <maths/quaternion.h>
#include <maths/matrix22.h>
#include <math.h>

namespace gef
{
	const Matrix33 Matrix33::kIdentity =	{1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f};
	const Matrix33 Matrix33::kZero =		{0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f};


	float Matrix33::CalculateDeterminant() const
	{
		float temp[5];

		temp[0] =  m[0][0]*Matrix22::Determinant(m[1][1], m[1][2], m[2][1], m[2][2]);
		temp[1] = -m[0][1]*Matrix22::Determinant(m[1][0], m[1][2], m[2][0], m[2][2]);
		temp[2] =  m[0][2]*Matrix22::Determinant(m[1][0], m[1][1], m[2][0], m[2][1]);

		//float deter = (m[0][0]*m[1][1]*m[2][2]+m[0][1]*m[1][2]*m[2][0]+m[0][2]*m[1][0]*m[2][1])-(m[0][2]*m[1][1]*m[2][0]+m[0][1]*m[1][0]*m[2][2]+m[0][0]*m[1][2]*m[2][1]);

		return temp[0]+temp[1]+temp[2];
	}

	void Matrix33::Transpose(const Matrix33& matrix)
	{
		for(Int32 rowNum=0;rowNum<3;++rowNum)
			for(Int32 columnNum=0;columnNum<3;++columnNum)
				m[rowNum][columnNum] = matrix.m[columnNum][rowNum];
	}

	void Matrix33::Cofactor(const Matrix33 matrix)
	{
		m[0][0] = +Matrix22::Determinant(matrix.m[1][1], matrix.m[1][2], matrix.m[2][1], matrix.m[2][2]);
		m[0][1] = -Matrix22::Determinant(matrix.m[1][0], matrix.m[1][2], matrix.m[2][0], matrix.m[2][2]);
		m[0][2] = +Matrix22::Determinant(matrix.m[1][0], matrix.m[1][1], matrix.m[2][0], matrix.m[2][1]);

		m[1][0] = -Matrix22::Determinant(matrix.m[0][1], matrix.m[0][2], matrix.m[2][1], matrix.m[2][2]);
		m[1][1] = +Matrix22::Determinant(matrix.m[0][0], matrix.m[0][2], matrix.m[2][0], matrix.m[2][2]);
		m[1][2] = -Matrix22::Determinant(matrix.m[0][0], matrix.m[0][1], matrix.m[2][0], matrix.m[2][1]);

		m[2][0] = +Matrix22::Determinant(matrix.m[0][1], matrix.m[0][2], matrix.m[1][1], matrix.m[1][2]);
		m[2][1] = -Matrix22::Determinant(matrix.m[0][0], matrix.m[0][2], matrix.m[1][0], matrix.m[1][2]);
		m[2][2] = +Matrix22::Determinant(matrix.m[0][0], matrix.m[0][1], matrix.m[1][0], matrix.m[1][1]);
	}


	void Matrix33::Inverse(const Matrix33 matrix, float& determinant)
	{
		determinant = matrix.CalculateDeterminant();
		if(determinant != 0.0f)
		{
			float inv_d = 1.0f / determinant;

//			gef::Matrix33 cofactor;
//			cofactor.Cofactor(matrix);

//			Transpose(cofactor);
//			*this = *this * inv_d;

			m[0][0] = +Matrix22::Determinant(matrix.m[1][1], matrix.m[1][2], matrix.m[2][1], matrix.m[2][2])*inv_d;
			m[1][0] = -Matrix22::Determinant(matrix.m[1][0], matrix.m[1][2], matrix.m[2][0], matrix.m[2][2])*inv_d;
			m[2][0] = +Matrix22::Determinant(matrix.m[1][0], matrix.m[1][1], matrix.m[2][0], matrix.m[2][1])*inv_d;

			m[0][1] = -Matrix22::Determinant(matrix.m[0][1], matrix.m[0][2], matrix.m[2][1], matrix.m[2][2])*inv_d;
			m[1][1] = +Matrix22::Determinant(matrix.m[0][0], matrix.m[0][2], matrix.m[2][0], matrix.m[2][2])*inv_d;
			m[2][1] = -Matrix22::Determinant(matrix.m[0][0], matrix.m[0][1], matrix.m[2][0], matrix.m[2][1])*inv_d;

			m[0][2] = +Matrix22::Determinant(matrix.m[0][1], matrix.m[0][2], matrix.m[1][1], matrix.m[1][2])*inv_d;
			m[1][2] = -Matrix22::Determinant(matrix.m[0][0], matrix.m[0][2], matrix.m[1][0], matrix.m[1][2])*inv_d;
			m[2][2] = +Matrix22::Determinant(matrix.m[0][0], matrix.m[0][1], matrix.m[1][0], matrix.m[1][1])*inv_d;

		}
	}

	const Matrix33 Matrix33::operator*(const Matrix33& matrix) const
	{
		Matrix33 result;

		for (int i=0; i<3; i++)
			for (int j=0; j<3; j++)
				result.m[i][j] =  m[i][0] * matrix.m[0][j] + m[i][1] * matrix.m[1][j] + m[i][2] * matrix.m[2][j];

		return result; 
	}

	const Matrix33 Matrix33::operator*(const float scalar) const
	{
		Matrix33 result;

		for (int i=0; i<3; i++)
			for (int j=0; j<3; j++)
				result.m[i][j] =  m[i][j]*scalar;

		return result; 
	}

	void Matrix33::OrthoNormalise(const gef::Matrix33& matrix)
    {
      float len, temp[ 3 ][ 3 ];

      temp[ 0 ][ 0 ] = matrix.m[0][ 0 ];
      temp[ 0 ][ 1 ] = matrix.m[0][ 1 ];
      temp[ 0 ][ 2 ] = matrix.m[0][ 2 ];
      temp[ 1 ][ 0 ] = matrix.m[1][ 0 ];
      temp[ 1 ][ 1 ] = matrix.m[1][ 1 ];
      temp[ 1 ][ 2 ] = matrix.m[1][ 2 ];

      /* normalize x */
      len = sqrtf ( temp[ 0 ][ 0 ] * temp[ 0 ][ 0 ] + temp[ 0 ][ 1 ] * temp[ 0 ][ 1 ] + temp[ 0 ][ 2 ] * temp[ 0 ][ 2 ] );
      len = ( len == 0.0f ) ? 1.0f : 1.0f / len;
      temp[ 0 ][ 0 ] *= len;
      temp[ 0 ][ 1 ] *= len;
      temp[ 0 ][ 2 ] *= len;

      /* z = x cross y */
      temp[ 2 ][ 0 ] = temp[ 0 ][ 1 ] * temp[ 1 ][ 2 ] - temp[ 0 ][ 2 ] * temp[ 1 ][ 1 ];
      temp[ 2 ][ 1 ] = temp[ 0 ][ 2 ] * temp[ 1 ][ 0 ] - temp[ 0 ][ 0 ] * temp[ 1 ][ 2 ];
      temp[ 2 ][ 2 ] = temp[ 0 ][ 0 ] * temp[ 1 ][ 1 ] - temp[ 0 ][ 1 ] * temp[ 1 ][ 0 ];

      /* normalize z */
      len = sqrtf ( temp[ 2 ][ 0 ] * temp[ 2 ][ 0 ] + temp[ 2 ][ 1 ] * temp[ 2 ][ 1 ] + temp[ 2 ][ 2 ] * temp[ 2 ][ 2 ] );
      len = ( len == 0.0f ) ? 1.0f : 1.0f / len;
      temp[ 2 ][ 0 ] *= len;
      temp[ 2 ][ 1 ] *= len;
      temp[ 2 ][ 2 ] *= len;

      /* y = z cross x */
      temp[ 1 ][ 0 ] = temp[ 2 ][ 1 ] * temp[ 0 ][ 2 ] - temp[ 2 ][ 2 ] * temp[ 0 ][ 1 ];
      temp[ 1 ][ 1 ] = temp[ 2 ][ 2 ] * temp[ 0 ][ 0 ] - temp[ 2 ][ 0 ] * temp[ 0 ][ 2 ];
      temp[ 1 ][ 2 ] = temp[ 2 ][ 0 ] * temp[ 0 ][ 1 ] - temp[ 2 ][ 1 ] * temp[ 0 ][ 0 ];

      /* normalize y */
      len = sqrtf ( temp[ 1 ][ 0 ] * temp[ 1 ][ 0 ] + temp[ 1 ][ 1 ] * temp[ 1 ][ 1 ] + temp[ 1 ][ 2 ] * temp[ 1 ][ 2 ] );
      len = ( len == 0.0f ) ? 1.0f : 1.0f / len;
      temp[ 1 ][ 0 ] *= len;
      temp[ 1 ][ 1 ] *= len;
      temp[ 1 ][ 2 ] *= len;

      /* update matrix4 */
      m[0][ 0 ] = temp[ 0 ][ 0 ];
      m[0][ 1 ] = temp[ 0 ][ 1 ];
      m[0][ 2 ] = temp[ 0 ][ 2 ];
      m[1][ 0 ] = temp[ 1 ][ 0 ];
      m[1][ 1 ] = temp[ 1 ][ 1 ];
      m[1][ 2 ] = temp[ 1 ][ 2 ];
      m[2][ 0 ] = temp[ 2 ][ 0 ];
      m[2][ 1 ] = temp[ 2 ][ 1 ];
      m[2][ 2 ] = temp[ 2 ][ 2 ];
    }
}
