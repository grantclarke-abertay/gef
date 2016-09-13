#ifndef _GEF_MATHS_PLANE_H
#define _GEF_MATHS_PLANE_H

#include <maths/vector4.h>
#include <maths/vector4.h>

namespace gef
{
	enum PlanePoint
	{
		PP_IN_FRONT,
		PP_BEHIND,
		PP_ON_PLANE
	};

	class Plane : public Vector4
	{
	public:
		Plane(float a, float b, float c, float d);

		void Normalise();

		inline void set_a(float a) { set_x(a); }
		inline void set_b(float b) { set_y(b); }
		inline void set_c(float c) { set_z(c); }
		inline void set_d(float d) { set_w(d); }

		inline float a() const { return x(); }
		inline float b() const { return y(); }
		inline float c() const { return z(); }
		inline float d() const { return w(); }

		inline float DistanceFromPoint(const Vector4& point) const { return a()*point.x() + b()*point.y() + c()*point.z() + d(); }

		inline PlanePoint ClassifyPoint(const Vector4& point) const
		{
			float distance = DistanceFromPoint(point);
			if (distance < 0.0f)
				return PP_BEHIND;
			else if (distance > 0.0f)
				return PP_IN_FRONT;
			else
				return PP_ON_PLANE;
		}
	};
}
#endif // _GEF_MATHS_PLANE_H
