#ifndef _GEF_MATRIX_22_H
#define _GEF_MATRIX_22_H

namespace gef
{
	class Matrix22
	{
	public:
		inline static float Determinant(const float a, const float b, const float c, const float d) { return a*d-b*c; } 
	};
}

#endif // _GEF_MATRIX_22_H
