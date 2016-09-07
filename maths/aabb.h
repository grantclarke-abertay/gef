#ifndef _GEF_AABB_H
#define _GEF_AABB_H

#include <maths/vector3.h>

namespace gef
{
	class Aabb
	{
	public:
		Aabb();
		Aabb(const Vector3& min_vtx, const Vector3& max_vtx);
		void Update(const Vector3& point);
		const Aabb Transform(const Matrix44& transform_matrix) const;

		inline void set_min_vtx(const Vector3& min_vtx) { min_vtx_ = min_vtx; }
		inline const Vector3& min_vtx() const { return min_vtx_; }
		inline void set_max_vtx(const Vector3& max_vtx) { max_vtx_ = max_vtx; }
		inline const Vector3& max_vtx() const { return max_vtx_; }

	private:
		Vector3 min_vtx_;
		Vector3 max_vtx_;
		bool initialised_;
	};
}

#endif