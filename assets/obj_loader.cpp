#include <assets/obj_loader.h>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/model.h>
#include <assets/png_loader.h>
#include <graphics/texture.h>
#include <graphics/image_data.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <graphics/material.h>

#include <cstdio>
#include <cstring>
#include <istream>
#include <cfloat>

namespace gef
{


bool OBJLoader::Load(const char* filename, Platform& platform, Model& model)
{
	bool success = true;

	std::vector<Texture*> textures;
	std::vector<gef::Vector4> positions;
	std::vector<gef::Vector4> normals;
	std::vector<gef::Vector2> uvs;
	std::vector<Int32> face_indices;
	std::vector<Int32> primitive_indices;
	std::vector<Int32> texture_indices;

	std::map<std::string, Int32> materials;

	std::string obj_filename(filename);
	void* obj_file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();

	success = file->Open(obj_filename.c_str());
	if(success)
	{
		success = file->GetSize(file_size);
		if(success)
		{
			obj_file_data = malloc(file_size);
			success = obj_file_data != NULL;
			if(success)
			{
				Int32 bytes_read;
				success = file->Read(obj_file_data, file_size, bytes_read);
				if(success)
					success = bytes_read == file_size;
			}
		}
	}

	if(!success)
	{
		free(obj_file_data);
		obj_file_data = NULL;

		delete file;
		file = NULL;
		return false;
	}
	gef::MemoryStreamBuffer buffer((char*)obj_file_data, file_size);
	std::istream stream(&buffer);

	{
		while( !stream.eof() )
		{
		    char line[128];
			stream >> line;
			if ( strcmp( line, "mtllib" ) == 0 )
			{
				char material_filename[256];
				stream >> material_filename;

				LoadMaterials(platform, material_filename, materials, textures);
			}

			// vertices
			else if ( strcmp( line, "v" ) == 0 )
			{
				float x, y, z;
				stream >> x;
				stream >> y;
				stream >> z;
				positions.push_back(gef::Vector4(x, y, z));
			}

			// normals
			else if ( strcmp( line, "vn" ) == 0 )
			{
				float nx, ny, nz;
				stream >> nx;
				stream >> ny;
				stream >> nz;
				normals.push_back(gef::Vector4(nx, ny, nz));
			}

			// uvs
			else if ( strcmp( line, "vt" ) == 0 )
			{
				float u, v;
				stream >> u;
				stream >> v;
				uvs.push_back(gef::Vector2(u, v));
			}

			else if(strcmp( line, "usemtl" ) == 0)
			{

				char material_name[256];
				stream >> material_name;

				// any time the material is changed
				// a new primitive is created
				primitive_indices.push_back((Int32)face_indices.size());

				texture_indices.push_back(materials[material_name]);
			}
			else if ( strcmp( line, "f" ) == 0 )
			{
				Int32 vertexIndex[3], uvIndex[3], normalIndex[3];
/*
    //std::string vertex1, vertex2, vertex3;
				Int32 matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
				if (matches != 9)
				{
//					printf("File can't be read by our simple parser : ( Try exporting with other options\n");
					success = false;
				}
*/

				stream >> vertexIndex[0]; stream.ignore(); stream >> uvIndex[0]; stream.ignore(); stream >> normalIndex[0];
				stream >> vertexIndex[1]; stream.ignore(); stream >> uvIndex[1]; stream.ignore(); stream >> normalIndex[1];
				stream >> vertexIndex[2]; stream.ignore(); stream >> uvIndex[2]; stream.ignore(); stream >> normalIndex[2];

				face_indices.push_back(vertexIndex[2]);
				face_indices.push_back(uvIndex[2]);
				face_indices.push_back(normalIndex[2]);

				face_indices.push_back(vertexIndex[1]);
				face_indices.push_back(uvIndex[1]);
				face_indices.push_back(normalIndex[1]);

				face_indices.push_back(vertexIndex[0]);
				face_indices.push_back(uvIndex[0]);
				face_indices.push_back(normalIndex[0]);
			}
		}

		// don't need the font file data any more
		free(obj_file_data);
		obj_file_data = NULL;

		// finished reading the file
//		fclose(file);
		// start building the mesh
		Int32 num_faces = (Int32)face_indices.size() / 9;
		Int32 num_vertices = num_faces*3;

		// create vertex buffer
		gef::Mesh::Vertex* vertices = new gef::Mesh::Vertex[num_vertices];

		// need to record min and max position values for mesh bounds
		gef::Vector4 pos_min(FLT_MAX, FLT_MAX, FLT_MAX), pos_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);

		for(Int32 vertex_num = 0; vertex_num < num_vertices; ++vertex_num)
		{
			gef::Mesh::Vertex* vertex = &vertices[vertex_num];
			gef::Vector4 position = positions[face_indices[vertex_num*3]-1];
			gef::Vector2 uv = uvs[face_indices[vertex_num*3+1]-1];
			gef::Vector4 normal = normals[face_indices[vertex_num*3+2]-1];

			vertex->px = position.x();
			vertex->py = position.y();
			vertex->pz = position.z();
			vertex->nx = normal.x();
			vertex->ny = normal.y();
			vertex->nz = normal.z();
			vertex->u = uv.x;
			vertex->v = -uv.y;

			// update min and max positions for bounds
			if (position.x() < pos_min.x())
				pos_min.set_x(position.x());
			if (position.y() < pos_min.y())
				pos_min.set_y(position.y());
			if (position.z() < pos_min.z())
				pos_min.set_z(position.z());
			if (position.x() > pos_max.x())
				pos_max.set_x(position.x());
			if (position.y() > pos_max.y())
				pos_max.set_y(position.y());
			if (position.z() > pos_max.z())
				pos_max.set_z(position.z());
		}


		Mesh* mesh = new Mesh(platform);
		model.set_mesh(mesh);
		model.set_textures(textures);

		// set bounds
		gef::Aabb aabb(pos_min, pos_max);
		gef::Sphere sphere(aabb);
		mesh->set_aabb(aabb);
		mesh->set_bounding_sphere(sphere);


		// create materials for each texture
		for(std::vector<Texture*>::iterator texture=textures.begin(); texture != textures.end(); ++texture)
		{
			Material* material = new Material();
			material->set_texture(*texture);
			model.AddMaterial(material);
		}

		mesh->InitVertexBuffer(platform, vertices, num_vertices, sizeof(gef::Mesh::Vertex));

		// create primitives
		mesh->AllocatePrimitives((UInt32)primitive_indices.size());

		std::vector<UInt32*> indices;
		indices.resize(primitive_indices.size());
		for(UInt32 primitive_num=0;primitive_num<primitive_indices.size();++primitive_num)
		{
			Int32 index_count = 0;

			if(primitive_num == primitive_indices.size()-1)
				index_count = (Int32)face_indices.size() - primitive_indices[primitive_num];
			else
				index_count = primitive_indices[primitive_num+1] - primitive_indices[primitive_num];

			// 9 indices per triangle, index count is the number of vertices in this primitive
			index_count /= 3;

			indices[primitive_num] = new UInt32[index_count];

			for(Int32 index=0;index<index_count;++index)
				indices[primitive_num][index] = primitive_indices[primitive_num]+index;

			mesh->GetPrimitive(primitive_num)->set_type(gef::TRIANGLE_LIST);
			mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num], index_count, sizeof(UInt32));
//			mesh->GetPrimitive(primitive_num)->InitIndexBuffer(platform, indices[primitive_num], 3, sizeof(UInt32));


			Int32 texture_index = texture_indices[primitive_num];
			if(texture_index == -1)
				mesh->GetPrimitive(primitive_num)->set_material(NULL);
			else
				mesh->GetPrimitive(primitive_num)->set_material(model.material(texture_index));
			//mesh->GetPrimitive(primitive_num)->set_texture(textures_[0]);
		}

		// mesh construction complete
		// clean up
		DeleteArrayNull(vertices);
		for(UInt32 primitive_num=0;primitive_num<indices.size();++primitive_num)
			DeleteArrayNull(indices[primitive_num]);

	}
	return success;
}

bool OBJLoader::LoadMaterials(Platform& platform, const char* filename, std::map<std::string, Int32>& materials, std::vector<Texture*>& textures)
{
	PNGLoader png_loader;

	bool success = true;


	std::string mtl_filename(filename);
	void* mtl_file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();
	success = file->Open(mtl_filename.c_str());
	if(success)
	{
		success = file->GetSize(file_size);
		if(success)
		{
			mtl_file_data = malloc(file_size);
			success = mtl_file_data != NULL;
			if(success)
			{
				Int32 bytes_read;
				success = file->Read(mtl_file_data, file_size, bytes_read);
				if(success)
					success = bytes_read == file_size;
			}
		}
	}

	if(!success)
	{
		free(mtl_file_data);
		mtl_file_data = NULL;

		delete file;
		file = NULL;
		return false;
	}
	gef::MemoryStreamBuffer buffer((char*)mtl_file_data, file_size);
	std::istream stream(&buffer);


	{
		std::map<std::string, std::string> material_name_mappings;
		char material_name[256];
		while( !stream.eof() )
		{
		    char line[128];
			stream >> line;

			if ( strcmp( line, "newmtl" ) == 0 )
			{
				stream >> material_name;
				material_name_mappings[material_name] = "";
			}
			else if(strcmp( line, "map_Kd" ) == 0)
			{
				char texture_name[256];
				stream >> texture_name;
				material_name_mappings[material_name] = texture_name;
			}
		}

		free(mtl_file_data);
		mtl_file_data = NULL;

		for(std::map<std::string, std::string>::iterator iter = material_name_mappings.begin(); iter != material_name_mappings.end(); ++iter)
		{
			if(iter->second.compare("") != 0)
			{
				gef::ImageData image_data;
				png_loader.Load(iter->second.c_str(), platform, image_data);
				Texture* texture = gef::Texture::Create(platform, image_data);
				textures.push_back(texture);
				materials[iter->first] = (Int32)textures.size()-1;
			}
			else
			{
				materials[iter->first] = -1;
			}
		}
	}

	return success;

}

}
