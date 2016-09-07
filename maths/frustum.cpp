#include <maths/frustum.h>
#include <maths/matrix44.h>
#include <maths/sphere.h>
#include <maths/aabb.h>
#include <math.h>

namespace gef
{
	//
	// http://www.flipcode.com/archives/Frustum_Culling.shtml
	//
	FrustumIntersect Frustum::Intersects(const Sphere& sphere) const
	{
		const Vector3& sphere_centre = sphere.position();
		float sphere_radius = sphere.radius();

			// calculate our distances to each of the planes
		for (int i = 0; i < 6; ++i)
		{
			const Plane& plane = planes_[i];
			// find the distance to this plane
			float distance = plane.DistanceFromPoint(sphere_centre);

			// if this distance is < -sphere.radius, we are outside
			if (distance < -sphere_radius)
				return FI_OUT;

			// else if the distance is between +- radius, then we intersect
			if (fabsf(distance) < sphere_radius)
				return FI_INTERSECTS;
		}

		// otherwise we are fully in view
		return FI_IN;
	}

	FrustumIntersect Frustum::Intersects(const Aabb& aabb) const
	{
		Vector3 corner[8];
		int total_in = 0;

		// get the corners of the box into the vCorner array
		corner[0].set_x(aabb.min_vtx().x()); corner[0].set_y(aabb.min_vtx().y()); corner[0].set_z(aabb.min_vtx().z());
		corner[1].set_x(aabb.min_vtx().x()); corner[1].set_y(aabb.min_vtx().y()); corner[1].set_z(aabb.max_vtx().z());
		corner[2].set_x(aabb.min_vtx().x()); corner[2].set_y(aabb.max_vtx().y()); corner[2].set_z(aabb.min_vtx().z());
		corner[3].set_x(aabb.min_vtx().x()); corner[3].set_y(aabb.max_vtx().y()); corner[3].set_z(aabb.max_vtx().z());
		corner[4].set_x(aabb.max_vtx().x()); corner[4].set_y(aabb.min_vtx().y()); corner[4].set_z(aabb.min_vtx().z());
		corner[5].set_x(aabb.max_vtx().x()); corner[5].set_y(aabb.min_vtx().y()); corner[5].set_z(aabb.max_vtx().z());
		corner[6].set_x(aabb.max_vtx().x()); corner[6].set_y(aabb.max_vtx().y()); corner[6].set_z(aabb.min_vtx().z());
		corner[7].set_x(aabb.max_vtx().x()); corner[7].set_y(aabb.max_vtx().y()); corner[7].set_z(aabb.max_vtx().z());

		// test all 8 corners against the 6 sides 
		// if all points are behind 1 specific plane, we are out
		// if we are in with all points, then we are fully in
		for (int p = 0; p < 6; ++p)
		{
			int in_count = 8;
			int point_in = 1;

			for (int i = 0; i < 8; ++i)
			{
				// test this point against the planes
				if (planes_[p].ClassifyPoint(corner[i]) == PP_BEHIND)
				{
					point_in = 0;
					--in_count;
				}
			}

			// were all the points outside of plane p?
			if(in_count == 0)
				return FI_OUT;

			// check if they were all on the right side of the plane
			total_in += point_in;
		}

		// so if iTotalIn is 6, then all are inside the view
		if (total_in == 6)
			return FI_IN;

		// we must be partly in then otherwise
		return FI_INTERSECTS;

	}

	//
	// http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
	//

	void Frustum::ExtractPlanesD3D(const Matrix44& viewproj, bool normalise)
	{
		// Left clipping plane
		planes_[0].set_a(viewproj.m(0,3) + viewproj.m(0,0));
		planes_[0].set_b(viewproj.m(1,3) + viewproj.m(1,0));
		planes_[0].set_c(viewproj.m(2,3) + viewproj.m(2,0));
		planes_[0].set_d(viewproj.m(3,3) + viewproj.m(3,0));
		// Right clipping plane
		planes_[0].set_a(viewproj.m(0,3) - viewproj.m(0,0));
		planes_[0].set_b(viewproj.m(1,3) - viewproj.m(1,0));
		planes_[0].set_c(viewproj.m(2,3) - viewproj.m(2,0));
		planes_[0].set_d(viewproj.m(3,3) - viewproj.m(3,0));
		// Top clipping plane
		planes_[1].set_a(viewproj.m(0,3) - viewproj.m(0,1));
		planes_[1].set_b(viewproj.m(1,3) - viewproj.m(1,1));
		planes_[1].set_c(viewproj.m(2,3) - viewproj.m(2,1));
		planes_[1].set_d(viewproj.m(3,3) - viewproj.m(3,1));
		// Bottom clipping plane
		planes_[2].set_a(viewproj.m(0,3) + viewproj.m(0,1));
		planes_[2].set_b(viewproj.m(1,3) + viewproj.m(1,1));
		planes_[2].set_c(viewproj.m(2,3) + viewproj.m(2,1));
		planes_[2].set_d(viewproj.m(3,3) + viewproj.m(3,1));
		// Near clipping plane
		planes_[3].set_a(viewproj.m(0,2));
		planes_[3].set_b(viewproj.m(1,2));
		planes_[3].set_c(viewproj.m(2,2));
		planes_[3].set_d(viewproj.m(3,2));
		// Far clipping plane
		planes_[5].set_a(viewproj.m(0,3) - viewproj.m(0,2));
		planes_[5].set_b(viewproj.m(1,3) - viewproj.m(1,2));
		planes_[5].set_c(viewproj.m(2,3) - viewproj.m(2,2));
		planes_[5].set_d(viewproj.m(3,3) - viewproj.m(3,2));
		// Normalize the plane equations, if requested
		if (normalise == true)
		{
			planes_[0].Normalise();
			planes_[1].Normalise();
			planes_[2].Normalise();
			planes_[3].Normalise();
			planes_[4].Normalise();
			planes_[5].Normalise();
		}
	}


	void Frustum::ExtractPlanesGL(const Matrix44& viewproj, bool normalise)
	{
		// Left clipping plane
		planes_[0].set_a(viewproj.m(3,0) + viewproj.m(0,0));
		planes_[0].set_b(viewproj.m(3,1) + viewproj.m(0,1));
		planes_[0].set_c(viewproj.m(3,2) + viewproj.m(0,2));
		planes_[0].set_d(viewproj.m(3,3) + viewproj.m(0,3));
		// Right clipping plane
		planes_[0].set_a(viewproj.m(3,0) - viewproj.m(0,0));
		planes_[0].set_b(viewproj.m(3,1) - viewproj.m(0,1));
		planes_[0].set_c(viewproj.m(3,2) - viewproj.m(0,2));
		planes_[0].set_d(viewproj.m(3,3) - viewproj.m(0,3));
		// Top clipping plane
		planes_[1].set_a(viewproj.m(3,0) - viewproj.m(1,0));
		planes_[1].set_b(viewproj.m(3,1) - viewproj.m(1,1));
		planes_[1].set_c(viewproj.m(3,2) - viewproj.m(1,2));
		planes_[1].set_d(viewproj.m(3,3) - viewproj.m(1,3));
		// Bottom clipping plane
		planes_[2].set_a(viewproj.m(3,0) + viewproj.m(1,0));
		planes_[2].set_b(viewproj.m(3,1) + viewproj.m(1,1));
		planes_[2].set_c(viewproj.m(3,2) + viewproj.m(1,2));
		planes_[2].set_d(viewproj.m(3,3) + viewproj.m(1,3));
		// Near clipping plane
		planes_[3].set_a(viewproj.m(3,0) + viewproj.m(2,0));
		planes_[3].set_b(viewproj.m(3,1) + viewproj.m(2,1));
		planes_[3].set_c(viewproj.m(3,2) + viewproj.m(2,2));
		planes_[3].set_d(viewproj.m(3,3) + viewproj.m(2,3));
		// Far clipping plane
		planes_[5].set_a(viewproj.m(3,0) - viewproj.m(2,0));
		planes_[5].set_b(viewproj.m(3,1) - viewproj.m(2,1));
		planes_[5].set_c(viewproj.m(3,2) - viewproj.m(2,2));
		planes_[5].set_d(viewproj.m(3,3) - viewproj.m(2,3));
		// Normalize the plane equations, if requested
		if (normalise == true)
		{
			planes_[0].Normalise();
			planes_[1].Normalise();
			planes_[2].Normalise();
			planes_[3].Normalise();
			planes_[4].Normalise();
			planes_[5].Normalise();
		}
	}
}