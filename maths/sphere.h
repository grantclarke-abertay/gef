#ifndef _GEF_SPHERE_H
#define _GEF_SPHERE_H

#include <maths/vector3.h>

namespace gef
{
	// forward declarations
	class Matrix44;
	class Aabb;

	class Sphere
	{
	public:
		Sphere();
		Sphere(const Vector3& position, const float radius);
		Sphere(const Aabb& aabb);
		inline void set_position(const Vector3& position) {position_ = position; }
		const Vector3& position() const { return position_; }
		inline void set_radius(const float radius) {radius_ = radius; }
		float radius() const { return radius_; }

		const Sphere Transform(const Matrix44& transform_matrix) const;
	private:
		Vector3 position_;
		float radius_;
	};
}

#endif // _GEF_SPHERE_H
