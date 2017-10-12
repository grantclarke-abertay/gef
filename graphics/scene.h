#ifndef _GEF_SCENE_H
#define _GEF_SCENE_H

#include <list>
#include <system/string_id.h>
#include <graphics/mesh_data.h>
#include <ostream>
#include <istream>
#include <map>

namespace gef
{
	class Mesh;
	class Texture;
	class Animation;
	class Platform;
	class Material;

	class Scene
	{
	public:
		~Scene();

		Mesh* CreateMesh(Platform& platform, const MeshData& mesh_data, const bool read_only = true);
		void CreateMeshes(Platform& platform, const bool read_only = true);
		void CreateMaterials(const Platform& platform);

		bool WriteSceneToFile(const Platform& platform, const char* filename) const;
		bool ReadSceneFromFile(const Platform& platform, const char* filename);

		bool ReadScene(std::istream& Stream);
		bool WriteScene(std::ostream& Stream) const;
//		void WriteStringTable(std::istream& Stream) const;
//		void ReadStringTable(std::istream& Stream);

		class Skeleton* FindSkeleton(const MeshData& mesh_data);
		void FixUpSkinWeights();

		Animation* FindAnimation(const gef::StringId anim_name_id);

		std::list<MeshData> mesh_data;
		std::list<MaterialData> material_data;
		std::list<Mesh*> meshes;
		std::list<Texture*> textures;
		std::list<Material*> materials;
		std::list<Skeleton*> skeletons;
		std::map<gef::StringId, Animation*> animations;
		StringIdTable string_id_table;

		std::map<gef::StringId, MaterialData*> material_data_map;
		std::map<gef::StringId, Material*> materials_map;
		std::map<gef::StringId, Texture*> textures_map;

		std::vector<gef::StringId> skin_cluster_name_ids;
	};
}

#endif // _GEF_SCENE_H