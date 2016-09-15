#ifndef _GEF_AABB_H
#define _GEF_AABB_H

#include <maths/vector4.h>

namespace gef
{
	/**
	An axis aligned bounding box represented by the minimum and maximum bounds.
	*/
	class Aabb
	{
	public:
		/// @brief Default constructor.
		/// @note The minimum bounds are initialised to largest possible positive float values
		/// and maximum bounds are initialised to largest possible negative values so the initial call to the Aabb::Update function
		/// will result in the bounds being overwritten.
		Aabb();

		/// @brief Creates an AABB given the minimum and maxium vertex position.
		/// @param[in] min_vtx		The minimum vertex position.
		/// @param[in] max_vtx		The maximum vertex position.
		Aabb(const Vector4& min_vtx, const Vector4& max_vtx);

		/// @brief Update this AABB bounds to include the position of a point.
		/// @param[in] point		The point to be included by the AABB bounds.
		void Update(const Vector4& point);

		/// @brief Transforms the bounds of the AABB by a transformation matrix.
		/// @param[in] transform_matrix		The matrix to transform this AABB.
		/// @return The transformed AABB.
		const Aabb Transform(const Matrix44& transform_matrix) const;

		/// @brief Sets the minimum bounds of the AABB.
		/// @param[in] min_vtx		The minimum bounds.
		inline void set_min_vtx(const Vector4& min_vtx) { min_vtx_ = min_vtx; }

		/// @brief Get the minimum bounds of the AABB.
		/// @return The minimum bounds.
		inline const Vector4& min_vtx() const { return min_vtx_; }

		/// @brief Sets the maximum bounds of the AABB.
		/// @param[in] max_vtx		The maximum bounds.
		inline void set_max_vtx(const Vector4& max_vtx) { max_vtx_ = max_vtx; }

		/// @brief Get the maximum bounds of the AABB
		/// @return The maximum bounds.
		inline const Vector4& max_vtx() const { return max_vtx_; }

	private:
		/// Minimum bounds.
		Vector4 min_vtx_;

		/// Maximum bounds.
		Vector4 max_vtx_;
	};
}

#endif