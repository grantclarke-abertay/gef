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



struct BoneInfo
{
	gef::Matrix44 BoneOffset;
	gef::Matrix44 FinalTransformation;

	std::string ParentBoneName;
	aiMesh* Mesh;
	int gef_joint_num;

	BoneInfo()
	{
		BoneOffset.SetZero();
		FinalTransformation.SetZero();
		Mesh = nullptr;
		gef_joint_num = -1;
	}
};

class AssimpSceneLoader
{
public:
	AssimpSceneLoader();
	~AssimpSceneLoader();
	bool ReadAssets(const char* filename, gef::Scene* scene, gef::Platform* platform);
	bool ReadAnimation(const char* filename, gef::Scene* scene);

	void ProcessAnimation(unsigned int anim_num, gef::Skeleton* skeleton);

	void SampleAnim(float anim_time_secs, gef::Skeleton* skeleton, float start_time, std::vector<gef::TransformAnimNode *> anim_nodes, aiAnimation* animation);

	void ProcessSkeletons();

	void Close();

	void AddBone(gef::Skeleton* skeleton, std::string& parent_bone_name, int parent_bone_index);


	void BoneTransform(float TimeInSeconds, std::vector<gef::Matrix44>& Transforms, aiScene* animation_scene, aiAnimation* animation, gef::Skeleton* skeleton);
	void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const gef::Matrix44& ParentTransform, aiScene* animation_scene, aiAnimation* animation);


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

	inline bool ignore_skinning() const { return ignore_skinning_; }
	inline void set_ignore_skinning(bool val) { ignore_skinning_ = val; }
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
	bool ignore_skinning_;

	std::map<std::string, BoneInfo> bone_info_;
	std::map<int, std::string> joint_num_to_names_;
public:



protected:

	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data, int start_vertex);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data);
	gef::MaterialData ProcessMaterial(aiMaterial* material, const aiScene* scene, const char* name);

	void CalcInterpolatedScaling(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedRotation(aiQuaternion& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	void CalcInterpolatedPosition(aiVector3D& Out, float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
	unsigned int FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
	const aiNodeAnim* FindNodeAnim(const aiAnimation* pAnimation, const std::string NodeName);
};

#endif // ifndef _ASSIMP_SCENE_LOADER_H
