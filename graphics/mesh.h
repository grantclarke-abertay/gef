#ifndef _GEF_MESH_H
#define _GEF_MESH_H

#include <gef.h>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector4.h>
#include <maths/vector2.h>
#include <maths/aabb.h>
#include <maths/sphere.h>

namespace gef
{
	class Platform;
	class VertexBuffer;

	class Mesh
	{
	public:
		struct Vertex
		{
			//Vector4 position;
			//Vector4 normal;
			//Vector2 uv;
			float px;
			float py;
			float pz;
			float nx;
			float ny;
			float nz;
			float u;
			float v;
		};

		struct SkinnedVertex
		{
			float px;
			float py;
			float pz;
			float nx;
			float ny;
			float nz;
			UInt8 bone_indices[4]; 
			float bone_weights[4];
			float u;
			float v;
		};

		Mesh(Platform& platform);
		virtual ~Mesh();
		virtual bool InitVertexBuffer(Platform& platform, const void* vertices, const UInt32 num_vertices, const UInt32 vertex_byte_size, const bool read_only = true);
//		virtual bool UpdateVertices(class Platform& platform, const void* vertices) = 0;
		void AllocatePrimitives(const UInt32 num_primitives);

		inline const class Primitive* GetPrimitive(UInt32 index) const	{ return primitives_[index];	}
		inline class Primitive* GetPrimitive(UInt32 index)				{ return const_cast<class Primitive*>(static_cast<const Mesh&>(*this).GetPrimitive(index));	}
		inline UInt32 num_primitives() const {return num_primitives_;}

//		inline UInt32 num_vertices() const { return num_vertices_; }
//		inline UInt32 vertex_byte_size() const { return vertex_byte_size_; }

		inline void set_aabb(const gef::Aabb& aabb) { aabb_ = aabb; }
		inline void set_bounding_sphere(const gef::Sphere& sphere) { bounding_sphere_ = sphere; }

		inline const Aabb& aabb() const { return aabb_; }
		inline const Sphere& bounding_sphere() const { return bounding_sphere_; }

		inline const VertexBuffer* vertex_buffer() const { return vertex_buffer_; }
		inline VertexBuffer* vertex_buffer() { return vertex_buffer_; }

		static Mesh* Create(Platform& platform);

	protected:
		virtual class Primitive* AllocatePrimitive(); // move to platform class?
		void ReleasePrimitives();
		void CalculateBounds(const Vector4* vertex_positions, const UInt32 num_vertices, const UInt32 vertex_byte_size);

		UInt32 num_primitives_;
		class Primitive ** primitives_;
//		UInt32 num_vertices_;
//		UInt32 vertex_byte_size_;
		Aabb aabb_;
		Sphere bounding_sphere_;
		VertexBuffer* vertex_buffer_;
		Platform& platform_;
	};
}
#endif // _GEF_MESH_H
