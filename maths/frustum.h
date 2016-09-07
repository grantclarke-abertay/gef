#ifndef _GEF_MATHS_FRUSTUM_H
#define _GEF_MATHS_FRUSTUM_H

#include <maths/plane.h>

namespace gef
{
	class Matrix44;
	class Sphere;
	class Aabb;

	enum FrustumPlane
	{
		FP_LEFT = 0,
		FP_RIGHT,
		FP_TOP,
		FP_BOTTOM,
		FP_NEAR,
		FP_FAR,
		NUM_FRUSTUM_PLANES
	};

	enum FrustumIntersect
	{
		FI_OUT = 0,
		FI_IN,
		FI_INTERSECTS
	};
	class Frustum
	{
	public:
		FrustumIntersect Intersects(const Sphere& sphere) const;
		FrustumIntersect Intersects(const Aabb& aabb) const;
		void ExtractPlanesD3D(const Matrix44& viewproj, bool normalise);
		void ExtractPlanesGL(const Matrix44& viewproj, bool normalise);
	protected:
		Plane planes_[NUM_FRUSTUM_PLANES];
	};
}

#endif // _GEF_MATHS_FRUSTUM_H
