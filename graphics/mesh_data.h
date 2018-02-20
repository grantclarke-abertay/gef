#ifndef _GEF_MESH_DATA_H
#define _GEF_MESH_DATA_H

#include <vector>
#include <maths/vector4.h>
#include <graphics/primitive.h>
#include <string>
#include <system/string_id.h>
#include <maths/aabb.h>

#include <ostream>
#include <istream>

namespace gef
{
	struct MaterialData
	{
		std::string diffuse_texture;
		gef::StringId name_id;
		UInt32 colour;

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;
	};

	struct PrimitiveData
	{
		PrimitiveData();
		~PrimitiveData();

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

		void* indices;
		//MaterialData* material;
		gef::StringId material_name_id;
		Int32 num_indices;
		Int32 index_byte_size;
		PrimitiveType type;
	};

	struct VertexData
	{
		VertexData();
		~VertexData();

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

		void* vertices;
		Int32 num_vertices;
		Int32 vertex_byte_size;
	};


	struct MeshData
	{
		MeshData();
		~MeshData();

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

		VertexData vertex_data;
		std::vector<PrimitiveData*> primitives;
		gef::StringId name_id;

		Aabb aabb;
	};
}

#endif // _GEF_MESH_DATA_H
