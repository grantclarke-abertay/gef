#ifndef _ASSIMP_SCENE_LOADER_H
#define _ASSIMP_SCENE_LOADER_H

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <graphics/mesh_data.h>

namespace gef
{
	class Scene;
	class Mesh;
	class Platform;
}

class GameObject;

//struct aiNode;
//struct aiScene;
//struct aiMesh;

class AssimpSceneLoader
{
public:
	AssimpSceneLoader();
	~AssimpSceneLoader();
	bool ReadAssets(const char* filename, gef::Scene* scene, gef::Platform* platform);
	void Close();

	inline const gef::Aabb &get_scene_aabb() const
	{
		return scene_aabb_;
	}


	inline bool flip_winding_order() const { return flip_winding_order_; }
	inline void set_flip_winding_order(bool val) { flip_winding_order_ = val; }


	inline bool make_left_handed() const { return make_left_handed_; }
	inline void set_make_left_handed(bool val) { make_left_handed_ = val; }
protected:
	gef::Scene* output_scene_;
	gef::Platform* platform_;

	Assimp::Importer* importer_;
	const aiScene* assimp_scene_;
	gef::Aabb scene_aabb_;

	bool flip_winding_order_;
	bool make_left_handed_;


protected:

	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data, int start_vertex);
	void ProcessMesh(aiMesh* mesh, const aiScene* scene, gef::MeshData& mesh_data);
	gef::MaterialData ProcessMaterial(aiMaterial* material, const aiScene* scene, const char* name);
};

#endif // ifndef _ASSIMP_SCENE_LOADER_H
