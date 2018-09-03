#include "assimp_scene_loader.h"
#include <graphics/mesh.h>
#include <graphics/scene.h>
#include <animation/animation.h>
#include <animation/skeleton.h>
#include <system/platform.h>
#include <vector>
#include <cassert>


#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <maths/math_utils.h>

#include <iostream>

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

void set_gef_matrix_from_ai_matrix(gef::Matrix44& dest_matrix, const aiMatrix4x4& src_matrix)
{
	gef::Matrix44 temp(&src_matrix[0][0]);
	dest_matrix.Transpose(temp);
}

AssimpSceneLoader::AssimpSceneLoader()
	: output_scene_(nullptr)
	, assimp_scene_(nullptr)
	, assimp_anim_scene_(nullptr)
	, importer_(nullptr)
	, flip_winding_order_(false)
	, make_left_handed_(false)
	, generate_normals_(false)
	, rotate_90_xaxis_(false)
{

}

AssimpSceneLoader::~AssimpSceneLoader()
{
	Close();
}

void AssimpSceneLoader::Close()
{
	bone_info_.clear();
	joint_num_to_names_.clear();

	assimp_scene_ = nullptr;
	assimp_anim_scene_ = nullptr;

	delete importer_;
	importer_ = nullptr;
}

void AssimpSceneLoader::AddBone(gef::Skeleton * skeleton, std::string & parent_bone_name, int parent_bone_index)
{
	// go through all bones and add bones that have the same parent
	int prev_num_joints = skeleton->joint_count();

	for (auto bone_info_mapping = bone_info_.begin(); bone_info_mapping != bone_info_.end(); ++bone_info_mapping)
	{
		std::string bone_name = bone_info_mapping->first;
		BoneInfo& bone_info = bone_info_mapping->second;

		if (parent_bone_name.compare(bone_info.ParentBoneName) == 0)
		{
			gef::Joint joint;

			joint.name_id = output_scene_->string_id_table.Add(bone_name);
			joint.inv_bind_pose = bone_info.BoneOffset;
			joint.parent = parent_bone_index;

			bone_info.gef_joint_num = skeleton->joint_count();
			joint_num_to_names_[skeleton->joint_count()] = bone_name;
			skeleton->AddJoint(joint);

			// special case, if this is a root bone, stop searching as we only want one root per skeleton
			if (parent_bone_index == -1)
				break;
		}
	}

	int curr_num_joints = skeleton->joint_count();

	// now go through all newly add joints and add their children
	for (int joint_num = prev_num_joints; joint_num < curr_num_joints; ++joint_num)
	{
		int parent_joint_num = joint_num;
		std::string parent_bone_name = joint_num_to_names_[parent_joint_num];

		AddBone(skeleton, parent_bone_name, parent_joint_num);
	}
}

bool AssimpSceneLoader::ReadAssets(const char* filename, gef::Scene* scene, gef::Platform* platform)
{
	if (!scene || !platform)
		return false;
	
	// clear out previous bone mapping data
	bone_info_.clear();
	joint_num_to_names_.clear();

	importer_ = new Assimp::Importer;

	unsigned int import_flags = aiProcess_Triangulate;

	if (flip_winding_order_)
		import_flags |= aiProcess_FlipWindingOrder;

	if (make_left_handed_)
		import_flags |= aiProcess_MakeLeftHanded;

	// import file with no post processing
	assimp_scene_ = importer_->ReadFile(filename, 0);

	// generate normals force before doing anything else
	if(generate_normals_)
		importer_->ApplyPostProcessing(aiProcess_GenSmoothNormals);

	// do all other post processing
	importer_->ApplyPostProcessing(import_flags);


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

	// see if there is a skeleton in the imported scene
	ProcessSkeletons();

	int num_vertices = 0;
	bool skinned_mesh = false;

	int num_meshes = assimp_scene_->mNumMeshes;

	// just take first mesh at the moment
//	int num_meshes = assimp_scene_->mNumMeshes > 0 ? 1 : 0;

	for (int mesh_num = 0; mesh_num < num_meshes; ++mesh_num)
	{
		num_vertices += assimp_scene_->mMeshes[mesh_num]->mNumVertices;

		if (assimp_scene_->mMeshes[mesh_num]->HasBones())
			skinned_mesh = true;
	}

	// create meshes
	int start_vertex = 0;
	output_scene_->mesh_data.push_back(gef::MeshData());
	output_scene_->mesh_data.back().vertex_data.num_vertices = num_vertices;

	if (skinned_mesh)
	{
		output_scene_->mesh_data.back().vertex_data.vertex_byte_size = sizeof(gef::Mesh::SkinnedVertex);
		output_scene_->mesh_data.back().vertex_data.vertices = new gef::Mesh::SkinnedVertex[num_vertices];
	}
	else
	{
		output_scene_->mesh_data.back().vertex_data.vertex_byte_size = sizeof(gef::Mesh::Vertex);
		output_scene_->mesh_data.back().vertex_data.vertices = new gef::Mesh::Vertex[num_vertices];
	}

	for (int mesh_num = 0; mesh_num < num_meshes; ++mesh_num)
	{
		ProcessMesh(assimp_scene_->mMeshes[mesh_num], assimp_scene_, output_scene_->mesh_data.back(), start_vertex);
		start_vertex += assimp_scene_->mMeshes[mesh_num]->mNumVertices;
	}
	scene_aabb_ = output_scene_->mesh_data.back().aabb;

	delete importer_;
	importer_ = nullptr;

	return true;
}

bool AssimpSceneLoader::ReadAnimation(const char* filename, gef::Scene* scene)
{
	if (!scene)
		return false;

	importer_ = new Assimp::Importer;


	unsigned int import_flags = 0;

	if (make_left_handed_)
		import_flags |= aiProcess_MakeLeftHanded;


	assimp_anim_scene_ = importer_->ReadFile(filename, import_flags);

	if (!assimp_anim_scene_)
		return false;

	output_anim_scene_ = scene;

	for (unsigned int anim_num = 0; anim_num < assimp_anim_scene_->mNumAnimations; ++anim_num)
	{
		if (output_scene_->skeletons.size() > 0)
			ProcessAnimation(anim_num, output_scene_->skeletons.front());
	}

	delete importer_;
	importer_ = nullptr;


	return true;
}

void AssimpSceneLoader::ProcessAnimation(unsigned int anim_num, gef::Skeleton* skeleton)
{
	aiAnimation* src_anim = assimp_anim_scene_->mAnimations[anim_num];
	gef::Animation* output_anim = new gef::Animation();

	std::string anim_name = "animation" + std::to_string(anim_num);
	if (src_anim->mName.length > 0)
		anim_name = src_anim->mName.C_Str();
	gef::StringId anim_id = output_scene_->string_id_table.Add(anim_name);

	output_anim_scene_->animations[anim_id] = output_anim;

	// create transform nodes for each bone
	std::vector<gef::TransformAnimNode*> anim_nodes;
	for (int bone_num = 0; bone_num < skeleton->joint_count(); ++bone_num)
	{
		gef::TransformAnimNode* output_node = new gef::TransformAnimNode();

//		gef::StringId anim_node_id = output_scene_->string_id_table.Add(mesh_bones->FindBoneName(bone_num));
		output_node->set_name_id(skeleton->joint(bone_num).name_id);

		output_anim->AddNode(output_node);
		anim_nodes.push_back(output_node);
	}

	// sample animation every 1/60th second
	float tick_in_secs = 1.0f / 30.0f;
	float anim_duration_in_secs = (float)src_anim->mDuration/(float)src_anim->mTicksPerSecond;

	// go through all the anim channels and find the smallest and largest key time
	// this will be used as the start and end time
	float start_time = FLT_MAX;
	float end_time = -FLT_MAX;

	for (unsigned int channel_num = 0; channel_num < src_anim->mNumChannels; ++channel_num)
	{
		aiNodeAnim* node_anim = src_anim->mChannels[channel_num];

		for (unsigned int poskey_num=0; poskey_num < node_anim->mNumPositionKeys; ++poskey_num)
		{
			float time = (float)node_anim->mPositionKeys[poskey_num].mTime;
			if (time < start_time)
				start_time = time;
			if (time > end_time)
				end_time = time;
		}

		for (unsigned int rotkey_num = 0; rotkey_num < node_anim->mNumRotationKeys; ++rotkey_num)
		{
			float time = (float)node_anim->mRotationKeys[rotkey_num].mTime;
			if (time < start_time)
				start_time = time;
			if (time > end_time)
				end_time = time;
		}

		for (unsigned int sclkey_num = 0; sclkey_num < node_anim->mNumScalingKeys; ++sclkey_num)
		{
			float time = (float)node_anim->mScalingKeys[sclkey_num].mTime;
			if (time < start_time)
				start_time = time;
			if (time > end_time)
				end_time = time;
		}
	}

	anim_duration_in_secs = (end_time - start_time) / (float)src_anim->mTicksPerSecond;
	float start_time_in_secs = start_time / (float)src_anim->mTicksPerSecond;
	float end_time_in_secs = end_time / (float)src_anim->mTicksPerSecond;

	output_anim->set_start_time(0.0f);
	output_anim->set_end_time(anim_duration_in_secs);
	output_anim->CalculateDuration();

	for (float anim_time_secs = start_time_in_secs; anim_time_secs < end_time_in_secs; anim_time_secs += tick_in_secs)
		SampleAnim(anim_time_secs, skeleton, start_time, anim_nodes, src_anim);

	// add keys for end time
	SampleAnim(end_time_in_secs, skeleton, start_time, anim_nodes, src_anim);

}

void AssimpSceneLoader::SampleAnim(float anim_time_secs, gef::Skeleton* skeleton, float start_time, std::vector<gef::TransformAnimNode *> anim_nodes, aiAnimation* animation)
{
	std::vector<gef::Matrix44> joint_transforms(skeleton->joint_count());
	BoneTransform(anim_time_secs, joint_transforms, const_cast<aiScene*>(assimp_anim_scene_), animation, skeleton);


	for (int joint_num = 0; joint_num < skeleton->joint_count(); ++joint_num)
	{
		const gef::Matrix44& bone_world_transform = joint_transforms[joint_num];

		gef::Matrix44 parent_bone_transform;
		if (skeleton->joint(joint_num).parent == -1)
			parent_bone_transform.SetIdentity();
		else
			parent_bone_transform = joint_transforms[skeleton->joint(joint_num).parent];

		gef::Matrix44 inv_parent_bone_transform;
		inv_parent_bone_transform.AffineInverse(parent_bone_transform);

		gef::Matrix44 local_bone_transform = bone_world_transform * inv_parent_bone_transform;

		gef::Quaternion rotation;
		rotation.SetFromMatrix(local_bone_transform);
		rotation.Normalise();

		gef::Vector4 translation;
		translation = local_bone_transform.GetTranslation();

		gef::QuaternionKey rot_key;
		rot_key.time = anim_time_secs - start_time;
		rot_key.value = rotation;

		gef::Vector3Key pos_key;
		pos_key.time = anim_time_secs;
		pos_key.value = translation;

		gef::TransformAnimNode* anim_node = anim_nodes[joint_num];
		anim_node->rotation_keys().push_back(rot_key);
		anim_node->translation_keys().push_back(pos_key);
	}
}

void AssimpSceneLoader::ProcessSkeletons()
{
	// go through all meshes and find out which ones have bones attached to them
	for (size_t mesh_num = 0; mesh_num < assimp_scene_->mNumMeshes; mesh_num++)
	{
		aiMesh* mesh = assimp_scene_->mMeshes[mesh_num];
		if (mesh->HasBones())
		{
			for (unsigned int bone_num = 0; bone_num < mesh->mNumBones; ++bone_num)
			{
				aiBone* bone = mesh->mBones[bone_num];

				// add all bones
				std::string bone_name(bone->mName.data);
				if (bone_info_.find(bone_name) == bone_info_.end())
				{
					BoneInfo bone_info;
					set_gef_matrix_from_ai_matrix(bone_info.BoneOffset, bone->mOffsetMatrix);
					bone_info_[bone_name] = bone_info;
				}
			}

			// find parent bones
			{
				for (auto bone_info = bone_info_.begin(); bone_info != bone_info_.end(); ++bone_info)
				{
					std::string bone_name = bone_info->first;

					// find bone in scene
					aiNode* node = assimp_scene_->mRootNode->FindNode(bone_name.c_str());
					if (node)
					{
						bool parent_found = false;
						aiNode* parent_node = node->mParent;
						while (parent_node && !parent_found)
						{
							std::string parent_bone_name(parent_node->mName.data);

							auto parent_bone_info = bone_info_.find(parent_bone_name);
							if (parent_bone_info != bone_info_.end())
							{
								bone_info->second.ParentBoneName = parent_bone_name;
								parent_found = true;
							}
							else
								parent_node = parent_node->mParent;
						}
					}
				}
			}

			// construct skeletons
			{
				for (auto bone_info_mapping = bone_info_.begin(); bone_info_mapping != bone_info_.end(); ++bone_info_mapping)
				{
					std::string bone_name = bone_info_mapping->first;
					BoneInfo& bone_info = bone_info_mapping->second;
					std::string parent_bone_name = bone_info.ParentBoneName;

					if (parent_bone_name.size() == 0)
					{
						// we have a root bone
						gef::Skeleton* skeleton = new gef::Skeleton;
						AddBone(skeleton, parent_bone_name, -1);
						output_scene_->skeletons.push_back(skeleton);
					}
				}
			}
		}
	}
}



void AssimpSceneLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data, int start_vertex)
{
	// Data to fill
	std::vector<gef::Mesh::Vertex> vertices;
	std::vector<unsigned int> indices;

	gef::Matrix44 mesh_transform;
	mesh_transform.SetIdentity();

	if(rotate_90_xaxis_)
	{
		mesh_transform.RotationX(gef::DegToRad(-90.0f));
	}

	int sizeofVertex = 0;
	int posByteOffset = 0;
	int normalByteOffset = 0;
	int uvByteOffset = 0;
	int weightsByteOffset = 0;
	int bonesByteOffset = 0;

	bool skinned = mesh->HasBones();
//	skinned = false;

	if (skinned)
	{
		sizeofVertex = sizeof(gef::Mesh::SkinnedVertex);
		posByteOffset = 0;
		normalByteOffset = 12;
		bonesByteOffset = 24;
		weightsByteOffset = 28;
		uvByteOffset = 44;
	}
	else
	{
		sizeofVertex = sizeof(gef::Mesh::Vertex);
		posByteOffset = 0;
		normalByteOffset = 12;
		uvByteOffset = 24;
	}
	mesh_data.vertex_data.vertex_byte_size = sizeofVertex;


	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		UInt8* vertex_data = ((UInt8*)mesh_data.vertex_data.vertices) + (start_vertex + i) * sizeofVertex;

		gef::Vector4 position(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		gef::Vector2 uv(0.0f, 0.0f);
		gef::Vector4 normal(0.0f, 0.0f, 0.0f);

		if(rotate_90_xaxis_)
			position = position.Transform(mesh_transform);

		if (mesh->mTextureCoords[0])
		{
			uv.x = (float)mesh->mTextureCoords[0][i].x;
			uv.y = -(float)mesh->mTextureCoords[0][i].y;
		}

		if (mesh->HasNormals())
		{
			normal.set_value(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

			if(rotate_90_xaxis_)
				normal = normal.Transform(mesh_transform);
		}

		memcpy(vertex_data + posByteOffset, &position, sizeof(float) * 3);
		memcpy(vertex_data + normalByteOffset, &normal, sizeof(float) * 3);
		memcpy(vertex_data + uvByteOffset, &uv, sizeof(float) * 2);

		if (mesh->HasBones())
		{
			// zero out bone and skin data
			memset(vertex_data + bonesByteOffset, 0, sizeof(UInt8) * 4);
			memset(vertex_data + weightsByteOffset, 0, sizeof(float) * 4);
		}

		mesh_data.aabb.Update(gef::Vector4(position));
	}

	if (skinned)
	{
		// only supporting a single skeleton in the scene at the moment
		gef::Skeleton* skeleton = output_scene_->skeletons.front();
		for (unsigned int bone_num = 0; bone_num < mesh->mNumBones; ++bone_num)
		{
			const aiBone* src_bone = mesh->mBones[bone_num];

			src_bone->mName;

			src_bone->mOffsetMatrix;

			for (size_t weight_num = 0; weight_num < src_bone->mNumWeights; weight_num++)
			{
				aiVertexWeight& weight = src_bone->mWeights[weight_num];

				UInt8* vertex_data = ((UInt8*)mesh_data.vertex_data.vertices) + (start_vertex + weight.mVertexId) * sizeofVertex;

				gef::StringId joint_id = gef::GetStringId(src_bone->mName.C_Str());
				int joint_index = skeleton->FindJointIndex(joint_id);

				if (joint_index != -1)
				{
					// TODO Check number of influences

					// go through current influences and find first 0 weight to replace
					const int kMaxBoneInfluences = 4;

					float* weights = (float*)(vertex_data + weightsByteOffset);
					UInt8* bone_indices = vertex_data + bonesByteOffset;
					for (int influence_num = 0; influence_num < kMaxBoneInfluences; influence_num++)
					{
						if (*weights == 0.0f)
						{
							*weights = weight.mWeight;
							*bone_indices = joint_index;
							break;
						}
						else
						{
							weights++;
							bone_indices++;
						}
					}
				}
			}
		}
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(start_vertex+face.mIndices[j]);
	}

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

	aiColor3D diffuse_colour(1.f, 1.f, 1.f);
	material->Get(AI_MATKEY_COLOR_DIFFUSE, diffuse_colour);

	material_data.colour = gef::Colour(diffuse_colour.r, diffuse_colour.g, diffuse_colour.b).GetABGR();

	return material_data;
}



unsigned int AssimpSceneLoader::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	for (unsigned int i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumPositionKeys - 2;
}


unsigned int AssimpSceneLoader::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumRotationKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumRotationKeys - 2;
}


unsigned int AssimpSceneLoader::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	assert(pNodeAnim->mNumScalingKeys > 0);

	for (unsigned int i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++) {
		if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime) {
			return i;
		}
	}

	return pNodeAnim->mNumScalingKeys-2;
}


void AssimpSceneLoader::CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumPositionKeys == 1) {
		Out = pNodeAnim->mPositionKeys[0].mValue;
		return;
	}

	unsigned int PositionIndex = FindPosition(AnimationTime, pNodeAnim);
	unsigned int NextPositionIndex = (PositionIndex + 1);
	assert(NextPositionIndex < pNodeAnim->mNumPositionKeys);
	float DeltaTime = (float)(pNodeAnim->mPositionKeys[NextPositionIndex].mTime - pNodeAnim->mPositionKeys[PositionIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

	Factor = (Factor < 0.0f) ? 0.0f : ((Factor > 1.0f) ? 1.0f : Factor);

	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mPositionKeys[PositionIndex].mValue;
	const aiVector3D& End = pNodeAnim->mPositionKeys[NextPositionIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}


void AssimpSceneLoader::CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	// we need at least two values to interpolate...
	if (pNodeAnim->mNumRotationKeys == 1) {
		Out = pNodeAnim->mRotationKeys[0].mValue;
		return;
	}

	unsigned int RotationIndex = FindRotation(AnimationTime, pNodeAnim);
	unsigned int NextRotationIndex = (RotationIndex + 1);
	assert(NextRotationIndex < pNodeAnim->mNumRotationKeys);
	float DeltaTime = (float)(pNodeAnim->mRotationKeys[NextRotationIndex].mTime - pNodeAnim->mRotationKeys[RotationIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

	Factor = (Factor < 0.0f) ? 0.0f : ((Factor > 1.0f) ? 1.0f : Factor);

	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiQuaternion& StartRotationQ = pNodeAnim->mRotationKeys[RotationIndex].mValue;
	const aiQuaternion& EndRotationQ = pNodeAnim->mRotationKeys[NextRotationIndex].mValue;
	aiQuaternion::Interpolate(Out, StartRotationQ, EndRotationQ, Factor);
	Out = Out.Normalize();
}


void AssimpSceneLoader::CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim)
{
	if (pNodeAnim->mNumScalingKeys == 1) {
		Out = pNodeAnim->mScalingKeys[0].mValue;
		return;
	}

	unsigned int ScalingIndex = FindScaling(AnimationTime, pNodeAnim);
	unsigned int NextScalingIndex = (ScalingIndex + 1);
	assert(NextScalingIndex < pNodeAnim->mNumScalingKeys);
	float DeltaTime = (float)(pNodeAnim->mScalingKeys[NextScalingIndex].mTime - pNodeAnim->mScalingKeys[ScalingIndex].mTime);
	float Factor = (AnimationTime - (float)pNodeAnim->mScalingKeys[ScalingIndex].mTime) / DeltaTime;

	Factor = (Factor < 0.0f) ? 0.0f : ((Factor > 1.0f) ? 1.0f : Factor);

	assert(Factor >= 0.0f && Factor <= 1.0f);
	const aiVector3D& Start = pNodeAnim->mScalingKeys[ScalingIndex].mValue;
	const aiVector3D& End = pNodeAnim->mScalingKeys[NextScalingIndex].mValue;
	aiVector3D Delta = End - Start;
	Out = Start + Factor * Delta;
}




const aiNodeAnim* AssimpSceneLoader::FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName)
{
	if (pAnimation)
	{
		for (unsigned int i = 0; i < pAnimation->mNumChannels; i++) {
			const aiNodeAnim* pNodeAnim = pAnimation->mChannels[i];

			if (std::string(pNodeAnim->mNodeName.data) == NodeName) {
				return pNodeAnim;
			}
		}
	}

	return NULL;
}

void AssimpSceneLoader::BoneTransform(float TimeInSeconds, std::vector<gef::Matrix44>& Transforms, aiScene* animation_scene, aiAnimation* animation, gef::Skeleton* skeleton)
{
	gef::Matrix44 Identity;
	Identity.SetIdentity();

	float TicksPerSecond = 0.0f;
	if (animation)
		TicksPerSecond = (float)animation->mTicksPerSecond;

	float TimeInTicks = TimeInSeconds * TicksPerSecond;
	float AnimationTime = TimeInTicks;

	ReadNodeHeirarchy(AnimationTime, animation_scene->mRootNode, Identity, animation_scene, animation);

	for (int i = 0; i < skeleton->joint_count(); i++) {
		Transforms[i] = bone_info_[joint_num_to_names_[i]].FinalTransformation;
	}
}

void AssimpSceneLoader::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const gef::Matrix44& ParentTransform, aiScene* animation_scene, aiAnimation* animation)
{
	std::string NodeName(pNode->mName.data);

	const aiAnimation* pAnimation = animation;

	gef::Matrix44 NodeTransformation;
	set_gef_matrix_from_ai_matrix(NodeTransformation, pNode->mTransformation);

	const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);

	if (pNodeAnim) {
		// Interpolate scaling and generate scaling transformation matrix
		aiVector3D Scaling;
		CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
		gef::Matrix44 ScalingM;
		ScalingM.Scale(gef::Vector4(Scaling.x, Scaling.y, Scaling.z));

		// Interpolate rotation and generate rotation transformation matrix
		aiQuaternion RotationQ;
		CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);

		gef::Matrix44 RotationM;
		RotationM.Rotation(gef::Quaternion(RotationQ.x, RotationQ.y, RotationQ.z, RotationQ.w));

		// Interpolate translation and generate translation transformation matrix
		aiVector3D Translation;
		CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);

		gef::Matrix44 TranslationM;
		TranslationM.SetIdentity();
		TranslationM.SetTranslation(gef::Vector4(Translation.x, Translation.y, Translation.z));

		// Combine the above transformations
		NodeTransformation = ScalingM * RotationM * TranslationM;
	}

	gef::Matrix44 GlobalTransformation = NodeTransformation * ParentTransform;

	if (bone_info_.find(NodeName) != bone_info_.end()) {
		bone_info_.find(NodeName)->second.FinalTransformation = GlobalTransformation;
	}

	for (unsigned int i = 0; i < pNode->mNumChildren; i++) {
		ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation, animation_scene, animation);
	}
}
