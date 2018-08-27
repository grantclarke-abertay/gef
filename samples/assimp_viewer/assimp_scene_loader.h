#ifndef _ASSIMP_SCENE_LOADER_H
#define _ASSIMP_SCENE_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <graphics/mesh_data.h>
#include <maths/matrix44.h>


namespace gef
{
	class Scene;
	class Mesh;
	class Platform;
	class Skeleton;
	class TransformAnimNode;
}

class GameObject;

//struct aiNode;
//struct aiScene;
//struct aiMesh;

#define NUM_BONES_PER_VEREX 4

struct VertexBoneData
{
	unsigned int IDs[NUM_BONES_PER_VEREX];
	float Weights[NUM_BONES_PER_VEREX];

	VertexBoneData()
	{
		Reset();
	};

	void Reset()
	{
		memset(IDs, 0, sizeof(unsigned int)*NUM_BONES_PER_VEREX);
		memset(Weights, 0, sizeof(float)*NUM_BONES_PER_VEREX);
	}

//	void AddBoneData(unsigned int BoneID, float Weight);
};

struct BoneInfo
{
	gef::Matrix44 BoneOffset;
	gef::Matrix44 FinalTransformation;

	BoneInfo()
	{
		BoneOffset.SetZero();
		FinalTransformation.SetZero();
	}
};

struct MeshBones
{
	MeshBones();

	void LoadBones(/*unsigned int MeshIndex,*/ const aiMesh* pMesh/*, std::vector<VertexBoneData>& Bones*/);
	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const gef::Matrix44& ParentTransform, aiScene* animation_scene, aiAnimation* animation);
	void BoneTransform(float TimeInSeconds, std::vector<gef::Matrix44>& Transforms, aiScene* animation_scene, aiAnimation* animation);
	unsigned int FindParentBoneIndex(aiNode* bone_node);

	std::string FindBoneName(unsigned int bone_index);
	aiNode* FindBoneNode(aiNode* root_node, unsigned int bone_index);

	std::map<std::string, unsigned int> m_BoneMapping; // maps a bone name to its index
	std::map<unsigned int, int> m_JointMapping; // maps assimp bone index to gef joint index
	unsigned int m_NumBones;
	std::vector<BoneInfo> m_BoneInfo;
	gef::Matrix44 m_GlobalInverseTransform;
};

class AssimpSceneLoader
{
public:
	AssimpSceneLoader();
	~AssimpSceneLoader();
	bool ReadAssets(const char* filename, gef::Scene* scene, gef::Platform* platform);
	bool ReadAnimation(const char* filename, gef::Scene* scene);

	void ProcessAnimation(unsigned int anim_num, MeshBones* mesh_bones, gef::Skeleton* skeleton);

	void SampleAnim(MeshBones* mesh_bones, float anim_time_secs, gef::Skeleton* skeleton, float start_time, std::vector<gef::TransformAnimNode *> anim_nodes, aiAnimation* animation);

	void ProcessSkeletons();
	bool IsABone(aiNode* node, const std::map<std::string, aiBone*>& bone_nodes);
	aiNode* FindBoneNode(aiNode* node, const std::map<std::string, aiBone*>& bone_nodes);

	aiNode* FindParentBoneNode(aiNode* node, const std::map<std::string, aiBone*>& bone_nodes);

	void BuildSkeletonBones(aiNode* node, MeshBones& mesh_bones, int parent_joint_index, gef::Skeleton* skeleton);

	void Close();

	inline const gef::Aabb &get_scene_aabb() const
	{
		return scene_aabb_;
	}


	inline bool flip_winding_order() const { return flip_winding_order_; }
	inline void set_flip_winding_order(bool val) { flip_winding_order_ = val; }


	inline bool make_left_handed() const { return make_left_handed_; }
	inline void set_make_left_handed(bool val) { make_left_handed_ = val; }

	inline bool generate_normals() const { return generate_normals_; }
	inline void set_generate_normals(bool val) { generate_normals_ = val; }

    inline bool rotate_90_xaxis() const {
        return rotate_90_xaxis_;
    }

    inline void set_rotate_90_xaxis(bool val) {
        rotate_90_xaxis_ = val;
    }
protected:
	gef::Scene* output_scene_;
	gef::Scene* output_anim_scene_;
	gef::Platform* platform_;

	Assimp::Importer* importer_;
	const aiScene* assimp_scene_;
	const aiScene* assimp_anim_scene_;
	gef::Aabb scene_aabb_;

	bool flip_winding_order_;
	bool make_left_handed_;
	bool generate_normals_;
    bool rotate_90_xaxis_;

	std::vector<MeshBones*> mesh_bones_;



public:



protected:

	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data, int start_vertex);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data);
	gef::MaterialData ProcessMaterial(aiMaterial* material, const aiScene* scene, const char* name);



};

#endif // ifndef _ASSIMP_SCENE_LOADER_H
