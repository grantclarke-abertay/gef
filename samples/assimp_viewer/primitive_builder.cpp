#include "primitive_builder.h"
#include <graphics/mesh.h>
#include <system/platform.h>
#include <graphics/primitive.h>
#include <maths/math_utils.h>
#include <vector>
#include <math.h>
#include <system/debug_log.h>

//
// PrimitiveBuilder
//
PrimitiveBuilder::PrimitiveBuilder(gef::Platform& platform) :
	platform_(platform),
	default_cube_mesh_(NULL),
	default_sphere_mesh_(NULL)
{
	Init();
}

//
// ~PrimitiveBuilder
//
PrimitiveBuilder::~PrimitiveBuilder()
{
	CleanUp();
}

//
// Init
//
void PrimitiveBuilder::Init()
{
	// create helper geometry
	default_cube_mesh_ = CreateBoxMesh(gef::Vector4(0.5f, 0.5f, 0.5f));
	default_sphere_mesh_ = CreateSphereMesh(0.5f, 20, 20);

	// create materials for basic colours
	green_material_.set_colour(0xff4d7326);
	orange_material_.set_colour(0xff0066ff);
	grey_material_.set_colour(0xff575757);

}

//
// CleanUp
//
void PrimitiveBuilder::CleanUp()
{
	delete default_sphere_mesh_;
	default_sphere_mesh_ = NULL;

	delete default_cube_mesh_;
	default_cube_mesh_ = NULL;
}

//
// CreateBoxMesh
//
gef::Mesh* PrimitiveBuilder::CreateBoxMesh(const gef::Vector4& half_size, gef::Vector4 centre, gef::Material** materials)
{
	gef::Mesh* mesh = gef::Mesh::Create(platform_);

	//
	// vertices
	//
	// create vertices, 4 for each face so we have all vertices in a single vertex share the same normal
	const int kNumVertices = 4 * 6;
	gef::Mesh::Vertex vertices[kNumVertices] =
	{
		// front
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(),	centre.z() + half_size.z(), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(),	centre.z() + half_size.z(), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(), centre.z() + half_size.z(), 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(), centre.z() + half_size.z(), 0.0f, 0.0f, 1.0f, 1.0f, 1.0f },


		// back
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), 0.0f, 0.0f, -1.0f, 0.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), 0.0f, 0.0f, -1.0f, 1.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), 0.0f, 0.0f, -1.0f, 0.0f, 1.0f },
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), 0.0f, 0.0f, -1.0f, 1.0f, 1.0f },

		// left
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), -1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(),	centre.z() + half_size.z(), -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), -1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(), centre.z() + half_size.z(), -1.0f, 0.0f, 0.0f, 1.0f, 1.0f },


		// right
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(),	centre.z() + half_size.z(), 1.0f, 0.0f, 0.0f, 0.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), 1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(), centre.z() + half_size.z(), 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), 1.0f, 0.0f, 0.0f, 1.0f, 1.0f },


		// top
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), 0.0f, 1.0f, 0.0f, 0.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(),	centre.z() - half_size.z(), 0.0f, 1.0f, 0.0f, 1.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() + half_size.y(), centre.z() + half_size.z(), 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
		{ centre.x() + half_size.x(),	centre.y() + half_size.y(), centre.z() + half_size.z(), 0.0f, 1.0f, 0.0f, 1.0f, 1.0f },

		// bottom
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(),	centre.z() + half_size.z(), 0.0f, -1.0f, 0.0f, 0.0f, 0.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(),	centre.z() + half_size.z(), 0.0f, -1.0f, 0.0f, 1.0f, 0.0f },
		{ centre.x() - half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), 0.0f, -1.0f, 0.0f, 0.0f, 1.0f },
		{ centre.x() + half_size.x(),	centre.y() - half_size.y(), centre.z() - half_size.z(), 0.0f, -1.0f, 0.0f, 1.0f, 1.0f },
	};

	const int kNumIndices = 6 * 6;
	Int32 indices[kNumIndices] =
	{
		// front
		0, 1, 2,
		1, 3, 2,

		// back
		4, 5, 6,
		5, 7, 6,

		// left
		8, 9, 10,
		9, 11, 10,

		// right
		12, 13, 14,
		13, 15, 14,

		// top
		16, 17, 18,
		17, 19, 18,

		// bottom
		20, 21, 22,
		21, 23, 22
	};

	// create the vertex buffer for the box vertices
	mesh->InitVertexBuffer(platform_, vertices, kNumVertices, sizeof(gef::Mesh::Vertex));

	// create a primitive per face so we can alter the material per face
	const int num_faces = 6;
	mesh->AllocatePrimitives(num_faces);

	for (int primitive_num = 0; primitive_num < num_faces; ++primitive_num)
	{
		gef::Primitive* primitive = mesh->GetPrimitive(primitive_num);
		primitive->InitIndexBuffer(platform_, &indices[primitive_num*6], 6, sizeof(Int32));
		primitive->set_type(gef::TRIANGLE_LIST);

		// if materials pointer is valid then assume we have an array of Material pointers
		// with a size greater than 6 (one material per face)
		if(materials)
			primitive->set_material(materials[primitive_num]);
	}

	// set the bounds

	// axis aligned bounding box
	gef::Aabb aabb(centre - half_size, centre + half_size);
	mesh->set_aabb(aabb);

	// bounding sphere
	gef::Sphere sphere(aabb);
	mesh->set_bounding_sphere(sphere);
	
	return mesh;
}


//
// CalculateSphereSurfaceNormal
//
gef::Vector4 CalculateSphereSurfaceNormal(const float radius, const gef::Vector4& origin, const gef::Vector4& point)
{
	return gef::Vector4(
		(point.x() - origin.x()) / radius,
		(point.y() - origin.y()) / radius,
		(point.z() - origin.z()) / radius);
}

//
// CreateSphereMesh
//
// adapted from
// http://www.visualizationlibrary.org/documentation/_geometry_primitives_8cpp_source.html#l00284
gef::Mesh* PrimitiveBuilder::CreateSphereMesh(const float radius, const int phi, const int theta, gef::Vector4 origin, gef::Material* material)
{
	gef::Mesh* mesh = gef::Mesh::Create(platform_);

	const int kNumVertices = theta*phi + 2;
	std::vector<gef::Mesh::Vertex> vertices;
	vertices.resize(kNumVertices);


	int vert_idx = 0;
	gef::Vector4 position, normal(0.0f, 0.0f, 1.0f);
	gef::Vector2 uv(0.0f, 0.0f);

	position = origin + gef::Vector4(0.0f, radius, 0.0f);
	normal = CalculateSphereSurfaceNormal(radius, origin, position);

	vertices[vert_idx].px = position.x();
	vertices[vert_idx].py = position.y();
	vertices[vert_idx].pz = position.z();
	vertices[vert_idx].nx = normal.x();
	vertices[vert_idx].ny = normal.y();
	vertices[vert_idx].nz = normal.z();
	vertices[vert_idx].u = uv.x;
	vertices[vert_idx].v = uv.y;
	vert_idx++;
	for (int i = 0; i < theta; ++i)
	{
		for (int j = 0; j < phi; ++j)
		{
			gef::Vector4 v(0.0f, radius, 0.0f);

			gef::Matrix44 rotz, roty;

			rotz.RotationZ(gef::DegToRad(180.0f / (theta + 1)*(i + 1)));
			roty.RotationY(gef::DegToRad(360.0f / phi*j));

			v = v.Transform(rotz);
			v = v.Transform(roty);

			position = origin + v;
			normal = CalculateSphereSurfaceNormal(radius, origin, position);

			vertices[vert_idx].px = position.x();
			vertices[vert_idx].py = position.y();
			vertices[vert_idx].pz = position.z();
			vertices[vert_idx].nx = normal.x();
			vertices[vert_idx].ny = normal.y();
			vertices[vert_idx].nz = normal.z();
			vertices[vert_idx].u = uv.x;
			vertices[vert_idx].v = uv.y;
			vert_idx++;
		}
	}

	position = origin + gef::Vector4(0.0f, -radius, 0.0f);
	normal = CalculateSphereSurfaceNormal(radius, origin, position);

	vertices[vert_idx].px = position.x();
	vertices[vert_idx].py = position.y();
	vertices[vert_idx].pz = position.z();
	vertices[vert_idx].nx = normal.x();
	vertices[vert_idx].ny = normal.y();
	vertices[vert_idx].nz = normal.z();
	vertices[vert_idx].u = uv.x;
	vertices[vert_idx].v = uv.y;
	vert_idx++;


	mesh->InitVertexBuffer(platform_, &vertices[0], kNumVertices, sizeof(gef::Mesh::Vertex));
	mesh->AllocatePrimitives(2);

	// side quads
	std::vector<Int32> index_buffer;
	index_buffer.resize((theta - 1)*phi * 6);

	int idx = 0;
	for (int i = 0; i<theta - 1; ++i)
	{
		for (int j = 0; j<phi; ++j)
		{
			// 2 triangles per quad
			index_buffer.at(idx++) = 1 + phi*(i + 0) + (j + 1) % phi;
			index_buffer.at(idx++) = 1 + phi*(i + 1) + (j + 1) % phi;
			index_buffer.at(idx++) = 1 + phi*(i + 1) + (j + 0) % phi;

			index_buffer.at(idx++) = 1 + phi*(i + 0) + (j + 0) % phi;
			index_buffer.at(idx++) = 1 + phi*(i + 0) + (j + 1) % phi;
			index_buffer.at(idx++) = 1 + phi*(i + 1) + (j + 0) % phi;
		}
	}

	gef::Primitive* primitive;

	// setup primitive for side quads
	primitive = mesh->GetPrimitive(0);
	primitive->set_type(gef::TRIANGLE_LIST);
	primitive->set_material(material);
	primitive->InitIndexBuffer(platform_, &index_buffer[0], (UInt32)index_buffer.size(), sizeof(Int32));

	// top/bottom triangles
	index_buffer.resize(phi * 3 + phi * 3);

	idx = 0;
	// top fan
	for (int j = 0; j<phi; ++j)
	{
		index_buffer.at(idx++) = 1 + (j + 1) % phi;
		index_buffer.at(idx++) = 1 + (j + 0) % phi;
		index_buffer.at(idx++) = 0;
	}
	
	// bottom fan
	for (int j = 0; j<phi; ++j)
	{
		index_buffer.at(idx++) = 1 + phi*(theta - 1) + (j + 0) % phi;
		index_buffer.at(idx++) = 1 + phi*(theta - 1) + (j + 1) % phi;
		index_buffer.at(idx++) = (int)kNumVertices - 1;
	}

	// setup primitive for top and bottom fans
	primitive = mesh->GetPrimitive(1);
	primitive->set_type(gef::TRIANGLE_LIST);
	primitive->set_material(material);
	primitive->InitIndexBuffer(platform_, &index_buffer[0], (UInt32)index_buffer.size(), sizeof(Int32));

	// bounds
	gef::Aabb aabb(gef::Vector4(-radius, -radius, -radius) - origin, gef::Vector4(radius, radius, radius)+ origin);
	mesh->set_aabb(aabb);
	gef::Sphere sphere(origin, radius);
	mesh->set_bounding_sphere(sphere);

	return mesh;
}
