#include <maths/aabb.h>
#include <gef.h>
#include <cfloat>

namespace gef
{
	Aabb::Aabb() :
		min_vtx_(FLT_MAX, FLT_MAX, FLT_MAX),
		max_vtx_(-FLT_MAX, -FLT_MAX, -FLT_MAX)
	{
	}

	Aabb::Aabb(const Vector4& min_vtx, const Vector4& max_vtx)
	{
		min_vtx_ = min_vtx;
		max_vtx_ = max_vtx;
	}

	void Aabb::Update(const Vector4& point)
	{
		if(point.x() < min_vtx_.x())
			min_vtx_.set_x(point.x());
		if(point.y() < min_vtx_.y())
			min_vtx_.set_y(point.y());
		if(point.z() < min_vtx_.z())
			min_vtx_.set_z(point.z());
		if(point.x() > max_vtx_.x())
			max_vtx_.set_x(point.x());
		if(point.y() > max_vtx_.y())
			max_vtx_.set_y(point.y());
		if(point.z() > max_vtx_.z())
			max_vtx_.set_z(point.z());
	}

	const Aabb Aabb::Transform(const Matrix44& transform_matrix) const
	{
		// This can be optimised
		Aabb result;

		gef::Vector4 vertices[8];

		vertices[0].set_x(min_vtx_.x());
		vertices[0].set_y(min_vtx_.y());
		vertices[0].set_z(min_vtx_.z());
		vertices[1].set_x(max_vtx_.x());
		vertices[1].set_y(min_vtx_.y());
		vertices[1].set_z(min_vtx_.z());
		vertices[2].set_x(min_vtx_.x());
		vertices[2].set_y(max_vtx_.y());
		vertices[2].set_z(min_vtx_.z());
		vertices[3].set_x(max_vtx_.x());
		vertices[3].set_y(max_vtx_.y());
		vertices[3].set_z(min_vtx_.z());
		vertices[4].set_x(min_vtx_.x());
		vertices[4].set_y(min_vtx_.y());
		vertices[4].set_z(max_vtx_.z());
		vertices[5].set_x(max_vtx_.x());
		vertices[5].set_y(min_vtx_.y());
		vertices[5].set_z(max_vtx_.z());
		vertices[6].set_x(min_vtx_.x());
		vertices[6].set_y(max_vtx_.y());
		vertices[6].set_z(max_vtx_.z());
		vertices[7].set_x(max_vtx_.x());
		vertices[7].set_y(max_vtx_.y());
		vertices[7].set_z(max_vtx_.z());

		for(Int32 vertex_num=0; vertex_num < 8;++vertex_num)
			result.Update(vertices[vertex_num].Transform(transform_matrix));

		return result;
	}

}