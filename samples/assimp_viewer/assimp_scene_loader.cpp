#include "assimp_scene_loader.h"
#include <graphics/mesh.h>
#include <graphics/scene.h>
#include <system/platform.h>
#include <vector>


#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define aisgl_min(x,y) (x<y?x:y)
#define aisgl_max(x,y) (y>x?y:x)

/* ---------------------------------------------------------------------------- */
void get_bounding_box_for_node (const aiScene* scene,
		const aiNode* nd,
								aiVector3D* min,
								aiVector3D* max,
								aiMatrix4x4* trafo
){
	aiMatrix4x4 prev;
	unsigned int n = 0, t;

	prev = *trafo;
	aiMultiplyMatrix4(trafo,&nd->mTransformation);

	for (; n < nd->mNumMeshes; ++n) {
		const aiMesh* mesh = scene->mMeshes[nd->mMeshes[n]];
		for (t = 0; t < mesh->mNumVertices; ++t) {

			aiVector3D tmp = mesh->mVertices[t];
			aiTransformVecByMatrix4(&tmp,trafo);

			min->x = aisgl_min(min->x,tmp.x);
			min->y = aisgl_min(min->y,tmp.y);
			min->z = aisgl_min(min->z,tmp.z);

			max->x = aisgl_max(max->x,tmp.x);
			max->y = aisgl_max(max->y,tmp.y);
			max->z = aisgl_max(max->z,tmp.z);
		}
	}

	for (n = 0; n < nd->mNumChildren; ++n) {
		get_bounding_box_for_node(scene, nd->mChildren[n],min,max,trafo);
	}
	*trafo = prev;
}

/* ---------------------------------------------------------------------------- */
void get_bounding_box (const aiScene* scene, aiVector3D* min, aiVector3D* max)
{
	aiMatrix4x4 trafo;
	aiIdentityMatrix4(&trafo);

	min->x = min->y = min->z =  1e10f;
	max->x = max->y = max->z = -1e10f;
	get_bounding_box_for_node(scene, scene->mRootNode,min,max,&trafo);
}

AssimpSceneLoader::AssimpSceneLoader()
	: output_scene_(nullptr)
	, assimp_scene_(nullptr)
	, importer_(nullptr)
{

}

AssimpSceneLoader::~AssimpSceneLoader()
{
	Close();
}

void AssimpSceneLoader::Close()
{
	assimp_scene_ = nullptr;

	delete importer_;
	importer_ = nullptr;
}

bool AssimpSceneLoader::ReadAssets(const char* filename, gef::Scene* scene, gef::Platform* platform)
{
	if (!scene || !platform)
		return false;

	importer_ = new Assimp::Importer;
	// assets from unity are already in left handed coordinate space.
	// adding aiProcess_MakeLeftHanded will flip it to right handed which GEF uses.
	assimp_scene_ = importer_->ReadFile(filename, aiProcess_Triangulate | aiProcess_MakeLeftHanded);

	if (!assimp_scene_)
		return false;

	output_scene_ = scene;
	platform_ = platform;

	// create materials for the assimp_scene
	for (unsigned int material_num=0; material_num<assimp_scene_->mNumMaterials; ++material_num)
	{
		std::string material_name = filename + std::string(".mat.") + std::to_string(material_num);
		gef::StringId material_name_id = gef::GetStringId(material_name);
		output_scene_->string_id_table.Add(material_name);

		output_scene_->material_data.push_back(ProcessMaterial(assimp_scene_->mMaterials[material_num], assimp_scene_, material_name.c_str()));
		output_scene_->material_data_map[material_name_id] = &output_scene_->material_data.back();
	}

	// create meshes
	for (unsigned int mesh_num = 0; mesh_num < assimp_scene_->mNumMeshes; ++mesh_num)
	{
		output_scene_->mesh_data.push_back(gef::MeshData());

		ProcessMesh(assimp_scene_->mMeshes[mesh_num], assimp_scene_, output_scene_->mesh_data.back());
	}

	aiVector3D scene_min, scene_max, scene_center;

	get_bounding_box(assimp_scene_, &scene_min,&scene_max);
	scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
	scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
	scene_center.z = (scene_min.z + scene_max.z) / 2.0f;


	scene_aabb_.set_min_vtx(gef::Vector4(scene_min.x, scene_min.y, scene_min.z));
	scene_aabb_.set_max_vtx(gef::Vector4(scene_max.x, scene_max.y, scene_max.z));

	return true;
}

void AssimpSceneLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data)
{
	// Data to fill
	std::vector<gef::Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;
//	std::vector<Texture> textures;

	//if (mesh->mMaterialIndex >= 0)
	//{
	//	aiMaterial* mat = scene->mMaterials[mesh->mMaterialIndex];

	//	if (textype.empty()) textype = determineTextureType(scene, mat);
	//}

	// Walk through each of the mesh's vertices
	gef::Aabb aabb;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		gef::Mesh::Vertex vertex;

		vertex.px = mesh->mVertices[i].x;
		vertex.py = mesh->mVertices[i].y;
		vertex.pz = mesh->mVertices[i].z;

		if (mesh->mTextureCoords[0])
		{
			vertex.u = (float)mesh->mTextureCoords[0][i].x;
			vertex.v = -(float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			vertex.nx = mesh->mNormals[i].x;
			vertex.ny = mesh->mNormals[i].y;
			vertex.nz = mesh->mNormals[i].z;
		}

		vertices.push_back(vertex);

		aabb.Update(gef::Vector4(vertex.px, vertex.py, vertex.pz));
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	mesh_data.vertex_data.vertex_byte_size = sizeof(gef::Mesh::Vertex);
	mesh_data.vertex_data.num_vertices = mesh->mNumVertices;
	mesh_data.vertex_data.vertices = new gef::Mesh::Vertex[mesh_data.vertex_data.num_vertices];
	mesh_data.aabb = aabb;
	memcpy(mesh_data.vertex_data.vertices, &vertices[0], mesh_data.vertex_data.num_vertices * mesh_data.vertex_data.vertex_byte_size);

	gef::PrimitiveData* primitive_data = new gef::PrimitiveData();
	primitive_data->index_byte_size = sizeof(unsigned int);
	primitive_data->type = gef::TRIANGLE_LIST;
	primitive_data->num_indices = (Int32)indices.size();
	primitive_data->indices = new unsigned int[primitive_data->num_indices];

	// find name id of material
	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		std::list<gef::MaterialData>::iterator material_data_iter = output_scene_->material_data.begin();
		std::advance(material_data_iter, mesh->mMaterialIndex);
		primitive_data->material_name_id = material_data_iter->name_id;
	}
	else
		primitive_data->material_name_id = 0;


	memcpy(primitive_data->indices, &indices[0], primitive_data->num_indices * primitive_data->index_byte_size);
	mesh_data.primitives.push_back(primitive_data);
}

gef::MaterialData AssimpSceneLoader::ProcessMaterial(aiMaterial* material, const aiScene* scene, const char* name)
{
	gef::MaterialData material_data;

	material_data.name_id = gef::GetStringId(name);

	if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
	{
		aiString str;
		material->GetTexture(aiTextureType_DIFFUSE, 0, &str);
		material_data.diffuse_texture = str.C_Str();
	}
	else
		material_data.diffuse_texture = "";

	return material_data;
}
