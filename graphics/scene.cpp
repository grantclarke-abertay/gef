#include <graphics/scene.h>
#include <graphics/mesh.h>
#include <graphics/mesh_data.h>
#include <graphics/texture.h>
#include <animation/skeleton.h>
#include <animation/animation.h>
#include <system/platform.h>
#include <graphics/image_data.h>
#include <assets/png_loader.h>
#include <graphics/material.h>

#include <system/file.h>
#include <system/memory_stream_buffer.h>
#include <fstream>
#include <assert.h>

namespace gef
{
	Scene::~Scene()
	{
		// free up skeletons
		for(std::list<Skeleton*>::iterator skeleton_iter = skeletons.begin(); skeleton_iter != skeletons.end(); ++skeleton_iter)
			delete *skeleton_iter;

		// free up mesh_data
//		for(std::list<MeshData>::iterator mesh_iter = mesh_data.begin(); mesh_iter != mesh_data.end(); ++mesh_iter)
//			delete *mesh_iter;

		// free up textures
		for(std::list<Texture*>::iterator texture_iter = textures.begin(); texture_iter != textures.end(); ++texture_iter)
			delete *texture_iter;

		// free up materials
		for(std::list<Material*>::iterator material_iter = materials.begin(); material_iter != materials.end(); ++material_iter)
			delete *material_iter;

		// free up meshes
		for (std::list<Mesh*>::iterator mesh_iter = meshes.begin(); mesh_iter != meshes.end(); ++mesh_iter)
			delete *mesh_iter;


		// free up animations
		for(std::map<gef::StringId, Animation*>::iterator animation_iter = animations.begin(); animation_iter != animations.end(); ++animation_iter)
			delete animation_iter->second;
	}

	Mesh* Scene::CreateMesh(Platform& platform, const MeshData& mesh_data, const bool read_only)
	{
		return CreateMesh(platform, mesh_data, &materials_map, read_only);
	}


	Mesh* Scene::CreateMesh(Platform& platform, const MeshData& mesh_data, std::map<gef::StringId, Material*>* materials_map, const bool read_only)
	{
		Mesh* mesh = new Mesh(platform);
		mesh->set_aabb(mesh_data.aabb);
		mesh->set_bounding_sphere(gef::Sphere(mesh->aabb()));

		mesh->InitVertexBuffer(platform, mesh_data.vertex_data.vertices, mesh_data.vertex_data.num_vertices, mesh_data.vertex_data.vertex_byte_size, read_only);

		mesh->AllocatePrimitives((Int32)mesh_data.primitives.size());

		Int32 prim_index=0;
		for(std::vector<PrimitiveData*>::const_iterator prim_iter=mesh_data.primitives.begin();prim_iter != mesh_data.primitives.end();++prim_iter, ++prim_index)
		{
			Primitive* primitive = mesh->GetPrimitive(prim_index);
			primitive->set_type((*prim_iter)->type);
			primitive->InitIndexBuffer(platform, (*prim_iter)->indices, (*prim_iter)->num_indices, (*prim_iter)->index_byte_size, read_only);

			if (materials_map && (*prim_iter)->material_name_id != 0)
			{
				primitive->set_material((*materials_map)[(*prim_iter)->material_name_id]);
			}

			//if((*prim_iter)->material)
			//{
			//	if((*prim_iter)->material->diffuse_texture != "")
			//	{
			//		gef::StringId texture_name_id = gef::GetStringId((*prim_iter)->material->diffuse_texture);
			//		Texture* texture = textures_map[texture_name_id];
			//		if(texture)
			//			primitive->set_material(materials_map[(*prim_iter)->material->name_id]);
			//	}
			//}
		}

		return mesh;
	}

	void Scene::CreateMeshes(Platform& platform, const bool read_only)
	{
		for (std::list<MeshData>::const_iterator meshIter = mesh_data.begin(); meshIter != mesh_data.end(); ++meshIter)
		{
			meshes.push_back(CreateMesh(platform, *meshIter, read_only));
		}

	}


	void Scene::CreateMaterials(const Platform& platform)
	{

		// go through all the materials and create new textures for them
//		for(std::map<std::string, std::string>::iterator materialIter = materials_.begin();materialIter!=materials_.end();++materialIter)
		for(std::list<MaterialData>::iterator materialIter = material_data.begin();materialIter!=material_data.end();++materialIter)
		{
			Material* material = new Material();
			materials.push_back(material);
			materials_map[materialIter->name_id] = material;


			// colour
			material->set_colour(materialIter->colour);

			// texture
			if(materialIter->diffuse_texture != "")
			{
				gef::StringId texture_name_id = gef::GetStringId(materialIter->diffuse_texture);
				if(textures_map.find(texture_name_id) == textures_map.end())
				{
					string_id_table.Add(materialIter->diffuse_texture);

					ImageData image_data;
					PNGLoader png_loader;
					png_loader.Load(materialIter->diffuse_texture.c_str(), platform, image_data);
					if(image_data.image() != NULL)
					{
						Texture* texture = Texture::Create(platform, image_data);
						textures.push_back(texture);
						textures_map[texture_name_id] = texture;
						material->set_texture(texture);
					}
				}
			}
		}
	}


	bool Scene::WriteSceneToFile(const Platform& platform, const char* filename) const
	{
		bool success = true;

		std::ofstream file_stream(filename, std::ios::out | std::ios::binary);
		if(file_stream.is_open())
		{
			success = WriteScene(file_stream);
		}
		else
		{
			success = false;
		}

		file_stream.close();

		return success;
	}

	bool Scene::ReadSceneFromFile(const Platform& platform, const char* filename)
	{
		bool success = true;
		void* file_data = NULL;
		File* file = gef::File::Create();
		Int32 file_size;

		success = file->Open(filename);
		if(success)
		{
			success = file->GetSize(file_size);
			if(success)
			{
				file_data = malloc(file_size);
				success = file_data != NULL;
				if(success)
				{
					Int32 bytes_read;
					success = file->Read(file_data, file_size, bytes_read);
					if(success)
						success = bytes_read == file_size;
				}

				if(success)
				{
					gef::MemoryStreamBuffer stream_buffer((char*)file_data, file_size);

					std::istream input_stream(&stream_buffer);
					success = ReadScene(input_stream);

					// don't need the font file data any more
					free(file_data);
					file_data = NULL;
				}

			}

			file->Close();
		}

		delete file;
		return success;
	}

	bool Scene::ReadScene(std::istream& stream)
	{
		bool success = true;

		Int32 mesh_count;
		Int32 material_count;
		Int32 skeleton_count;
		Int32 animation_count;
		Int32 string_count;

		stream.read((char*)&mesh_count, sizeof(Int32));
		stream.read((char*)&material_count, sizeof(Int32));
		stream.read((char*)&skeleton_count, sizeof(Int32));
		stream.read((char*)&animation_count, sizeof(Int32));
		stream.read((char*)&string_count, sizeof(Int32));

		// string table
		for(Int32 string_num=0;string_num<string_count;++string_num)
		{
			std::string the_string = "";

			char string_character;
			do
			{
				stream.read(&string_character, 1);
				if(string_character != 0)
					the_string.push_back(string_character);
			}
			while(string_character != 0);

			string_id_table.Add(the_string);
		}

		// materials
		for(Int32 material_num=0;material_num<material_count;++material_num)
		{
			material_data.push_back(MaterialData());

			MaterialData& material = material_data.back();

			material.Read(stream);

			material_data_map[material.name_id] = &material;
		}

		// mesh_data
		for(Int32 mesh_num=0;mesh_num<mesh_count;++mesh_num)
		{
			mesh_data.push_back(MeshData());

			MeshData& mesh = mesh_data.back();

			mesh.Read(stream);

			// go through all primitives and try and find material to use
			//for(std::vector<PrimitiveData*>::iterator prim_iter =mesh.primitives.begin(); prim_iter != mesh.primitives.end(); ++prim_iter)
			//{
			//	gef::StringId material_name_id = (gef::StringId)((*prim_iter)->material->name_id);

			//	std::map<gef::StringId, MaterialData*>::const_iterator material_iter = material_data_map.find(material_name_id);
			//	if(material_iter != material_data_map.end())
			//		(*prim_iter)->material = material_iter->second;
			//}
		}

		// skeletons
		for(Int32 skeleton_num=0;skeleton_num<skeleton_count;++skeleton_num)
		{
			Skeleton* skeleton = new Skeleton();
			skeleton->Read(stream);
			skeletons.push_back(skeleton);
		}

		// animations
		for(Int32 animation_num=0;animation_num<animation_count;++animation_num)
		{
			Animation* animation = new Animation();
			animation->Read(stream);
			animations[animation->name_id()] = animation;
		}


		return success;
	}

	bool Scene::WriteScene(std::ostream& stream) const
	{
		bool success = true;

		Int32 mesh_count = (Int32)mesh_data.size();
		Int32 material_count = (Int32)material_data.size();
		Int32 skeleton_count = (Int32)skeletons.size();
		Int32 animation_count = (Int32)animations.size();
		Int32 string_count = (Int32)string_id_table.table().size();

		stream.write((char*)&mesh_count, sizeof(Int32));
		stream.write((char*)&material_count, sizeof(Int32));
		stream.write((char*)&skeleton_count, sizeof(Int32));
		stream.write((char*)&animation_count, sizeof(Int32));
		stream.write((char*)&string_count, sizeof(Int32));

		// string table
		for(std::map<gef::StringId, std::string>::const_iterator string_iter = string_id_table.table().begin(); string_iter != string_id_table.table().end(); ++string_iter)
		{
			//Int32 string_length = string_iter->second.length();
			stream.write(string_iter->second.c_str(), string_iter->second.length()+1);
		}

		// materials
		for(std::list<MaterialData>::const_iterator material_iter = material_data.begin(); material_iter != material_data.end(); ++material_iter)
			material_iter->Write(stream);

		// mesh_data
		for(std::list<MeshData>::const_iterator mesh_iter = mesh_data.begin(); mesh_iter != mesh_data.end(); ++mesh_iter)
			mesh_iter->Write(stream);

		// skeletons
		for(std::list<Skeleton*>::const_iterator skeleton_iter = skeletons.begin();skeleton_iter != skeletons.end(); ++skeleton_iter)
			(*skeleton_iter)->Write(stream);

		// animations
		for(std::map<gef::StringId, Animation*>::const_iterator animation_iter = animations.begin(); animation_iter != animations.end(); ++animation_iter)
			animation_iter->second->Write(stream);

		return success;
	}

	Skeleton* Scene::FindSkeleton(const MeshData& mesh_data)
	{
		Skeleton* result = NULL;

		if((mesh_data.vertex_data.num_vertices > 0) && (mesh_data.vertex_data.vertex_byte_size == sizeof(Mesh::SkinnedVertex)))
		{
			// get the first vertex
			Mesh::SkinnedVertex* skinned_vertex = (Mesh::SkinnedVertex*)mesh_data.vertex_data.vertices;

			// get string id of cluster link from first influence

			StringId joint_name_id = skin_cluster_name_ids[skinned_vertex->bone_indices[0]];

			// go through all skeletons looking a skeleton that contains the joint name
			for(std::list<Skeleton*>::iterator skeleton_iter = skeletons.begin(); skeleton_iter != skeletons.end(); ++skeleton_iter)
			{
				if((*skeleton_iter)->FindJoint(joint_name_id))
				{
					result = *skeleton_iter;
					break;
				}
			}
		}

		return result;
	}

	void Scene::FixUpSkinWeights()
	{
		for(std::list<MeshData>::iterator mesh_iter = mesh_data.begin(); mesh_iter != mesh_data.end(); ++mesh_iter)
		{
			if((mesh_iter->vertex_data.num_vertices > 0) && (mesh_iter->vertex_data.vertex_byte_size == sizeof(Mesh::SkinnedVertex)))
			{
				Skeleton* skeleton = FindSkeleton(*mesh_iter);
				if(skeleton)
				{
					Mesh::SkinnedVertex* skinned_vertices = (Mesh::SkinnedVertex*)mesh_iter->vertex_data.vertices;

					// go through all vertices and change cluster indices to joint indices
					for(Int32 vertex_num=0;vertex_num<mesh_iter->vertex_data.num_vertices;++vertex_num)
					{
						Mesh::SkinnedVertex* skinned_vertex = skinned_vertices+vertex_num;

						// calculate weight total to normalise weights
						float weight_total = skinned_vertex->bone_weights[0]+skinned_vertex->bone_weights[1]+skinned_vertex->bone_weights[2]+skinned_vertex->bone_weights[3];

						for(Int32 influence_index=0;influence_index < 4;++influence_index)
						{
							// normalise weight
							skinned_vertex->bone_weights[influence_index] /= weight_total;

							// fix up joint index
							Int32 joint_index = skeleton->FindJointIndex(skin_cluster_name_ids[skinned_vertex->bone_indices[influence_index]]);
							if(joint_index >= 0)
							{
								skinned_vertex->bone_indices[influence_index] = joint_index;
							}
							assert(joint_index >= 0);
						}
					}
				}
			}
		}
	}
}
