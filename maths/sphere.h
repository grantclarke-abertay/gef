#ifndef _GEF_SPHERE_H
#define _GEF_SPHERE_H

#include <maths/vector4.h>

namespace gef
{
	// forward declarations
	class Matrix44;
	class Aabb;

	/**
	A sphere represented by its centre position and radius
	*/
	class Sphere
	{
	public:
		/// @brief Default constructor.
		Sphere();

		/// @brief Creates a sphere given the position and radius.
		Sphere(const Vector4& position, const float radius);

		/// @brief Creates a sphere that encompasses the bounds off an axis aligned bounding box.
		Sphere(const Aabb& aabb);

		/// @brief Sets the centre position.
		/// @param[in] position		The centre position.
		inline void set_position(const Vector4& position) {position_ = position; }

		/// @brief Get the centre position.
		const Vector4& position() const { return position_; }

		/// @brief Sets the radius.
		/// @param[in] position		The centre position.
		inline void set_radius(const float radius) {radius_ = radius; }

		/// @brief Get the radius.
		float radius() const { return radius_; }

		/// @brief Transforms the sphere by a transformation matrix.
		/// @param[in] transform_matrix		The matrix to transform this AABB.
		/// @return The transformed sphere.
		const Sphere Transform(const Matrix44& transform_matrix) const;
	private:

		/// The centre position.
		Vector4 position_;

		/// The radius.
		float radius_;
	};
}

#endif // _GEF_SPHERE_H
