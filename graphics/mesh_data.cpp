#include <graphics/mesh_data.h>
#include <cstdlib>

namespace gef
{
	MeshData::MeshData()
	{
	}

	MeshData::~MeshData()
	{
		for(std::vector<PrimitiveData*>::iterator prim_iter = primitives.begin(); prim_iter != primitives.end(); ++prim_iter)
			delete (*prim_iter);
	}

	bool MeshData::Read(std::istream& stream)
	{
		bool success = true;

		Int32 primitive_count;
		gef::Vector4 aabb_min, aabb_max;


		stream.read((char*)&name_id, sizeof(StringId));
		stream.read((char*)&primitive_count, sizeof(Int32));
		stream.read((char*)&aabb_min, sizeof(gef::Vector4));
		stream.read((char*)&aabb_max, sizeof(gef::Vector4));

		vertex_data.Read(stream);

		for(Int32 prim_num=0;prim_num<primitive_count;++prim_num)
		{
			PrimitiveData* primitive_data = new PrimitiveData();
			primitive_data->Read(stream);
			primitives.push_back(primitive_data);

		}

		aabb.Update(aabb_min);
		aabb.Update(aabb_max);

		return success;
	}

	bool MeshData::Write(std::ostream& stream) const
	{
		bool success = true;

		Int32 primitive_count = (Int32)primitives.size();

		stream.write((char*)&name_id, sizeof(StringId));
		stream.write((char*)&primitive_count, sizeof(Int32));
		stream.write((char*)&aabb.min_vtx(), sizeof(gef::Vector4));
		stream.write((char*)&aabb.max_vtx(), sizeof(gef::Vector4));

		vertex_data.Write(stream);

		for(std::vector<PrimitiveData*>::const_iterator prim_iter = primitives.begin(); prim_iter != primitives.end(); ++prim_iter)
			(*prim_iter)->Write(stream);

		return success;
	}



	VertexData::VertexData() :
		vertices(NULL)
	{
	}

	VertexData::~VertexData()
	{
		if (vertices)
		{
			free(vertices);
			vertices = NULL;
		}
	}

	bool VertexData::Read(std::istream& stream)
	{
		bool success = true;

		stream.read((char*)&num_vertices, sizeof(Int32));
		stream.read((char*)&vertex_byte_size, sizeof(Int32));

		vertices = malloc(num_vertices*vertex_byte_size);
		if(vertices)
			stream.read((char*)vertices, num_vertices*vertex_byte_size);
		else
			success = false;

		return success;
	}

	bool VertexData::Write(std::ostream& stream) const
	{
		bool success = true;

		stream.write((char*)&num_vertices, sizeof(Int32));
		stream.write((char*)&vertex_byte_size, sizeof(Int32));
		stream.write((char*)vertices, num_vertices*vertex_byte_size);

		return success;
	}


	PrimitiveData::PrimitiveData() :
		indices(NULL),
		material_name_id(0)
	{
	}

	PrimitiveData::~PrimitiveData()
	{
		free(indices);
		indices = NULL;
	}

	bool PrimitiveData::Read(std::istream& stream)
	{
		bool success = true;

//		gef::StringId material_name_id = 0;
		// store material name id in material and fix up after whole mesh is loaded
		stream.read((char*)&material_name_id, sizeof(gef::StringId));
		stream.read((char*)&num_indices, sizeof(Int32));
		stream.read((char*)&index_byte_size, sizeof(Int32));
		stream.read((char*)&type, sizeof(PrimitiveType));

		indices = malloc(num_indices*index_byte_size);
		if(indices)
			stream.read((char*)indices, num_indices*index_byte_size);
		else
			success = false;

		return success;
	}

	bool PrimitiveData::Write(std::ostream& stream) const
	{
		bool success = true;

		//gef::StringId material_name_id = 0;
		//if(material)
		//	material_name_id = material->name_id;

		stream.write((char*)&material_name_id, sizeof(gef::StringId));
		stream.write((char*)&num_indices, sizeof(Int32));
		stream.write((char*)&index_byte_size, sizeof(Int32));
		stream.write((char*)&type, sizeof(PrimitiveType));
		stream.write((char*)indices, num_indices*index_byte_size);

		return success;
	}


	bool MaterialData::Read(std::istream& stream)
	{
		bool success = true;

		stream.read((char*)&name_id, sizeof(gef::StringId));
		stream.read((char*)&colour, sizeof(UInt32));

		char string_character;
		do
		{
			stream.read(&string_character, 1);
			if(string_character != 0)
				diffuse_texture.push_back(string_character);
		}
		while(string_character != 0);

		return success;
	}

	bool MaterialData::Write(std::ostream& stream) const
	{
		bool success = true;

		stream.write((char*)&name_id, sizeof(gef::StringId));
		stream.write((char*)&colour, sizeof(UInt32));

		Int32 string_length = (Int32)diffuse_texture.length();
		stream.write(diffuse_texture.c_str(), string_length+1);

		return success;
	}
}
