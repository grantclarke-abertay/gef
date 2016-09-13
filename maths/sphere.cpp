#include <maths/sphere.h>
#include <maths/aabb.h>

namespace gef
{
	Sphere::Sphere()
	{
	}

	Sphere::Sphere(const Vector4& position, const float radius) :
		position_(position),
		radius_(radius)
	{
	}

	Sphere::Sphere(const Aabb& aabb)
	{
		set_position((aabb.min_vtx()+aabb.max_vtx())*0.5f);
		float length = (aabb.max_vtx()-aabb.min_vtx()).Length();
		set_radius(length*0.5f);
	}

	const Sphere Sphere::Transform(const Matrix44& transform_matrix) const
	{
		Sphere result;

		Vector4 point_on_sphere(1.0f, 1.0f, 1.0f);
		point_on_sphere.Normalise();

		point_on_sphere *= radius_;

		point_on_sphere = point_on_sphere.TransformNoTranslation(transform_matrix);
		result.set_radius(point_on_sphere.Length());
		result.set_position(position_.Transform(transform_matrix));

		return result;
	}
}