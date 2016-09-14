#ifndef _GEF_AABB_H
#define _GEF_AABB_H

#include <maths/vector4.h>

namespace gef
{
	/** @remarks
			an axis aligned bounding box represented by the minimum and maximum bounds
	*/
	class Aabb
	{
	public:
		Aabb();

		/// @brief creates an AABB given the minimum and maxium vertex position
		/// @param[in] the minimum vertex position
		/// @param[in] the maximum vertex position
		Aabb(const Vector4& min_vtx, const Vector4& max_vtx);

		/// @brief update this AABB bounds to include the position of a point
		/// @param[in] the point to be included by the AABB bounds 
		void Update(const Vector4& point);

		/// @brief transform the bounds of the AABB by a transformation matrix
		/// @param[in] the matrix to transform this AABB
		const Aabb Transform(const Matrix44& transform_matrix) const;

		/// @brief sets the minimum bounds of the AABB
		inline void set_min_vtx(const Vector4& min_vtx) { min_vtx_ = min_vtx; }

		/// @brief returns the minimum bounds of the AABB
		inline const Vector4& min_vtx() const { return min_vtx_; }

		/// @brief sets the maximum bounds of the AABB
		inline void set_max_vtx(const Vector4& max_vtx) { max_vtx_ = max_vtx; }

		/// @brief returns the maximum bounds of the AABB
		inline const Vector4& max_vtx() const { return max_vtx_; }

	private:
		/// @brief minimum bounds
		Vector4 min_vtx_;

		/// @brief maximum bounds
		Vector4 max_vtx_;
	};
}

#endif