#include "fbx_loader.h"
#include <system/platform.h>
#include <graphics/scene.h>

#include <Windows.h>
#include <system/debug_log.h>
#include <iostream>

#define FBX_SUPPORTED
#ifdef FBX_SUPPORTED
#define FBXSDK_NEW_API
#include <fbxsdk.h>

#include <graphics/mesh.h>
#include <graphics/mesh_data.h>
#include <graphics/primitive.h>
#include <animation/skeleton.h>
#include <graphics/image_data.h>
#include <graphics/texture.h>
#include <assets/png_loader.h>
#include <animation/animation.h>
#include <animation/joint.h>
#include <animation/skeleton.h>


#include <vector>
#include <list>
#include <cassert>
#include <cstdlib>
#include <algorithm>

#endif

namespace gef
{
#ifdef FBX_SUPPORTED
	struct PolyVertData
	{
		int control_point_index;
		int vertex_index;
	};

	struct MeshVertexData
	{
		int uv_index;
		int normal_index;
		int vb_vert_index;

		MeshVertexData() :
			uv_index(-1),
			normal_index(-1),
			vb_vert_index(-1)
		{
		}
	};

	class ControlPoint
	{
	public:
		std::vector<gef::Vector4> normals;
		std::vector<gef::Vector2> uvs;
		std::vector<MeshVertexData> vertex_data;
		gef::Vector4 position;
		int count;
		int control_point_index;

		bool AddNormal(const gef::Vector4& new_normal, int& index)
		{
			bool value_found = false;
			index = 0;
			for (auto normal = normals.begin(); normal != normals.end(); ++normal, ++index)
			{
				// calculate the angle between the 2 normals, diff is the cos angle between 2 vectors
				float diff = normal->DotProduct(new_normal);
				
				const float kThreshold = 0.99f;
				if (diff >= kThreshold)
				{
					// normals are the same
					value_found = true;
					break;
				}
			}

			// normal wasn't found, add it to the list, index will be at the correct value
			if (!value_found)
				normals.push_back(new_normal);

			bool new_value = !value_found;
			return new_value;
		}

		bool AddUV(const gef::Vector2& new_uv, int& index)
		{
			bool value_found = false;
			index = 0;
			for (auto uv = uvs.begin(); uv != uvs.end(); ++uv, ++index)
			{
				// calculate distance between uvs to see how close they are
				float diff = (new_uv - *uv).LengthSqr();

				const float kThreshold = 0.0001f;
				if (diff <= kThreshold)
				{
					// uvs are the same
					value_found = true;
					break;
				}
			}

			// normal wasn't found, add it to the list, index will be at the correct value
			if (!value_found)
				uvs.push_back(new_uv);

			bool new_value = !value_found;
			return new_value;
		}

		bool AddVertexData(const Vector4& normal, const Vector2 uv, int& vertex_index)
		{
			vertex_index = 0;

			int normal_index;
			bool new_normal = AddNormal(normal, normal_index);

			int uv_index;
			bool new_uv = AddUV(uv, uv_index);

			// go through all the vertices to see if we have this combination
			bool found = false;

			for (auto vertex = vertex_data.begin(); vertex != vertex_data.end(); ++vertex, ++vertex_index)
			{
				if (vertex->normal_index == normal_index && vertex->uv_index == uv_index)
				{
					// vertex data found
					found = true;
					break;
				}
			}

			// new vertex
			if (!found)
			{
				MeshVertexData vertex;
				vertex.normal_index = normal_index;
				vertex.uv_index = uv_index;
				vertex_data.push_back(vertex);
			}

			bool new_value = !found;
			return new_value;
		}

		void SetVBVertexIndex(int vertex_index, int vb_vert_index)
		{
			vertex_data[vertex_index].vb_vert_index = vb_vert_index;
		}

		int GetVBVertexIndex(int vertex_index)
		{
			return vertex_data[vertex_index].vb_vert_index;
		}
	};


	struct MeshTriangle
	{
		Int32 index[3];
	};


	bool InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene);
	void DestroySdkObjects(FbxManager* manager);
	bool ReadFBXFile(FbxManager* manager, FbxDocument* scene, const char* const filename);

	void LoadScene(FbxScene& fbx_scene, class Scene& scene, Platform& platform);
	void LoadAsset(FbxNode& node, FbxScene& fbx_scene, class Scene& scene, Platform& platform);
	void AddMesh(FbxNode& node, Scene& scene, Platform& platform);
	void AddMeshConnections(FbxGeometry* geometry, const std::vector<std::list<Int32>>& control_point_vertices, MeshData& mesh, Scene& scene);
	void AddSkeleton(FbxNode& node, FbxScene& fbx_scene, Scene& scene, Skeleton* skeleton, std::vector<JointPose>& local_joint_transforms, FbxNode* parent_node = NULL);
	UInt32 GetMeshVertexCount(FbxMesh* fbx_mesh);
	Vector4 GetMeshVertexPosition(const FbxMesh* const fbx_mesh, const Int32 control_point_index);
	bool GetMeshVertexNormal(Vector4& normal, FbxMesh* const fbx_mesh, const Int32 control_point_index, const Int32 vertex_index);
	bool GetMeshVertexUV(Vector2& uv, FbxMesh* const fbx_mesh, const Int32 poly_index, const Int32 poly_vert_index, const Int32 control_point_index);
	Int32 GetMeshVertexUVIndex(FbxMesh* const fbx_mesh, const Int32 poly_index, const Int32 poly_vert_index, const Int32 control_point_index);
	Int32 GetMeshVertexUVCount(FbxMesh* const fbx_mesh);
	bool MeshUVsExist(FbxMesh* const fbx_mesh);
	const char* GetMeshMaterial(FbxMesh* const fbx_mesh, const Int32 poly_index, gef::Scene& scene);
	const char* GetMeshTexture(FbxMesh* const fbx_mesh, const Int32 poly_index, const Int32 element_index);
	void AddAnimation(FbxScene& fbx_scene, class Scene& scene);
	void AddAnimation(FbxNode& fbx_node, FbxAnimStack& fbx_anim_stack, class Animation& anim);
	void AddAnimation(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer, class Animation& anim);
	bool TranformKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer);
	bool ScaleKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer);
	bool RotationKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer);
	bool TranslationKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer);
//		bool BlendShapeChannelKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer);
//		void AddBlendShapeChannelKeys(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer, class Animation& anim);
	Vector4 GetVector3AnimCurveValue(const float time_in_secs, FbxPropertyT<FbxDouble3>& node_property, FbxAnimLayer& fbx_anim_layer);
//		void AddBlendShapes(FbxGeometry* geometry, class Mesh& mesh, Scene& scene);

static float fbx_scaling_factor_ = 1.f;
static const char* fbx_texture_filename_ext_ = NULL;
static bool fbx_ignore_skinning_ = false;
static bool fbx_strip_texture_path_ = false;
static FbxAMatrix fbx_axis_conversion_matrix_;
static FbxAxisSystem fbx_user_axis_system_;

static FbxManager* sdk_manager = NULL;
static FbxImporter* lImporter = NULL;

#endif

FBXLoader::FBXLoader() :
		scaling_factor_(1.0f),
		texture_filename_ext_(NULL),
		strip_texture_path_(false),
		ignore_skinning_(false),
		axis_system_set_(false)

{
}

FBXLoader::~FBXLoader()
{
}

void FBXLoader::SetAxisSystem(const fbxsdk::FbxAxisSystem& axis_system)
{
	axis_system_set_ = true;
	fbx_user_axis_system_ = axis_system;
}


bool FBXLoader::IsFileFormatSupported()
{
#ifdef FBX_SUPPORTED
	return true;
#else
	return false;
#endif
}

bool FBXLoader::Load(const char* const filename, Platform& platform, Scene& scene, const bool anim_data_only/* = false*/)
{
	bool success = true;

#ifdef FBX_SUPPORTED
	FbxScene* fbx_scene = NULL;

	fbx_scaling_factor_ = scaling_factor_;
	fbx_ignore_skinning_ = ignore_skinning_;
	fbx_strip_texture_path_ = strip_texture_path_;
	fbx_texture_filename_ext_ = texture_filename_ext_;

	// Prepare the FBX SDK.
	success = InitializeSdkObjects(sdk_manager, fbx_scene);
	if(success)
	{
		FbxSystemUnit sceneSystemUnit = fbx_scene->GetGlobalSettings().GetSystemUnit();

			// Load the FBX scene.
		success = ReadFBXFile(sdk_manager, fbx_scene, filename);

		sceneSystemUnit = fbx_scene->GetGlobalSettings().GetSystemUnit();

		if(sceneSystemUnit != FbxSystemUnit::cm)
		{
			FbxSystemUnit::cm.ConvertScene(fbx_scene);
		}


		FbxAxisSystem file_axis_system = fbx_scene->GetGlobalSettings().GetAxisSystem();

		int up_vector_sign, front_vector_sign;
		FbxAxisSystem::EUpVector up_vector;
		FbxAxisSystem::EFrontVector front_vector;
		FbxAxisSystem::ECoordSystem coord_system;

		up_vector = file_axis_system.GetUpVector(up_vector_sign);
		front_vector = file_axis_system.GetFrontVector(front_vector_sign);
		coord_system = file_axis_system.GetCoorSystem();

		// check to see if the user has overwritten the file axis system
		if(axis_system_set_)
			file_axis_system = fbx_user_axis_system_;

		up_vector = file_axis_system.GetUpVector(up_vector_sign);
		front_vector = file_axis_system.GetFrontVector(front_vector_sign);
		coord_system = file_axis_system.GetCoorSystem();

		// get matrix defined by file axis system
		FbxAMatrix file_axis_matrix, inv_file_axis_matrix;
		file_axis_system.GetMatrix(file_axis_matrix);
		inv_file_axis_matrix = file_axis_matrix.Inverse();

		// create axis system for used by gef
		FbxAxisSystem gefAxisSystem(
			FbxAxisSystem::EUpVector::eYAxis,
			FbxAxisSystem::EFrontVector::eParityOdd,
			FbxAxisSystem::eRightHanded);

		// get matrix for gef axis system
		FbxAMatrix gef_axis_matrix;
		gefAxisSystem.GetMatrix(file_axis_matrix);

		// calculate the matrix to transfer file axis system to gef axis system
		fbx_axis_conversion_matrix_ = inv_file_axis_matrix * gef_axis_matrix;

		if(success)
		{
			if(!anim_data_only)
			{
				LoadScene(*fbx_scene, scene, platform);
				scene.FixUpSkinWeights();
			}
			if(anim_data_only)
				AddAnimation(*fbx_scene, scene);

		}

		// Destroy the importer.
		if(lImporter)
		{
			lImporter->Destroy();
			lImporter = NULL;
		}

		DestroySdkObjects(sdk_manager);
	}
#else
	success = false;
#endif
	return success;
}

#ifdef FBX_SUPPORTED
bool InitializeSdkObjects(FbxManager*& manager, FbxScene*& scene)
{
	bool success = true;
    //The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
    manager = FbxManager::Create();
    if( !manager )
	{
        FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		success = false;
	}
	else
	{
//		FBXSDK_printf("Autodesk FBX SDK version %s\n", manager->GetVersion());

		//Create an IOSettings object. This object holds all import/export settings.
		FbxIOSettings* ios = FbxIOSettings::Create(manager, IOSROOT);
		manager->SetIOSettings(ios);

		//Load plugins from the executable directory (optional)
		//FbxString path = FbxGetApplicationDirectory();
		//manager->LoadPluginsDirectory(path.Buffer());

		//Create an FBX scene. This object holds most objects imported/exported from/to files.
		scene = FbxScene::Create(manager, "My Scene");
		if( !scene )
		{
		   FBXSDK_printf("Error: Unable to create FBX scene!\n");
		   success = false;
		}
	}

	return success;
}

void DestroySdkObjects(FbxManager* manager)
{
    //Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
    if( manager )
		manager->Destroy();
}

bool ReadFBXFile(FbxManager* manager, FbxDocument* scene, const char* const filename )
{
    int lFileMajor, lFileMinor, lFileRevision;
    int lSDKMajor,  lSDKMinor,  lSDKRevision;
    //int lFileFormat = -1;
    int i, lAnimStackCount;
    bool lStatus;
    char lPassword[1024];

    // Get the file version number generate by the FBX SDK.
    FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

    // Create an importer.
    lImporter = FbxImporter::Create(manager,"");

    // Initialize the importer by providing a filename.
    const bool lImportStatus = lImporter->Initialize(filename, -1, manager->GetIOSettings());
    lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

    if( !lImportStatus )
    {
        FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
/*
        FBXSDK_printf("Error returned: %s\n\n", lImporter->GetLastErrorString());

        if (lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedYet ||
            lImporter->GetLastErrorID() == FbxIOBase::eFileVersionNotSupportedAnymore)
        {
            FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
            FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", filename, lFileMajor, lFileMinor, lFileRevision);
        }
*/

        FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
        FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", filename, lFileMajor, lFileMinor, lFileRevision);
		
		return false;
    }

//    FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

    if (lImporter->IsFBX())
    {
        FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n", filename, lFileMajor, lFileMinor, lFileRevision);

        // From this point, it is possible to access animation stack information without
        // the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

        lAnimStackCount = lImporter->GetAnimStackCount();

        FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
        FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
        FBXSDK_printf("\n");

        for(i = 0; i < lAnimStackCount; i++)
        {
            FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

            FBXSDK_printf("    Animation Stack %d\n", i);
            FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
//            FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

            // Change the value of the import name if the animation stack should be imported 
            // under a different name.
            //FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

            // Set the value of the import state to false if the animation stack should be not
            // be imported. 
            //FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");

			FbxTime mStart = lTakeInfo->mLocalTimeSpan.GetStart();
			FbxTime mStop = lTakeInfo->mLocalTimeSpan.GetStop();

			FBXSDK_printf("         start time: %f\n", mStart.GetMilliSeconds() / 1000.f);
			FBXSDK_printf("         stop time: %f\n", mStop.GetMilliSeconds() / 1000.f);


            FBXSDK_printf("\n");
        }

		// Set the import states. By default, the import states are always set to 
        // true. The code below shows how to change these states.
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_MATERIAL,        true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_TEXTURE,         true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_LINK,            true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_SHAPE,           true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_GOBO,            true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_ANIMATION,       true);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
    }

    // Import the scene.
    lStatus = lImporter->Import(scene);

    // Get the list of all the animation stack.
	FbxArray<FbxString*> mAnimStackNameArray;
    scene->FillAnimStackNameArray(mAnimStackNameArray);
/*
    if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
    {
        FBXSDK_printf("Please enter password: ");

        lPassword[0] = '\0';

        scanf("%s", lPassword);

        FbxString lString(lPassword);

        manager->GetIOSettings()->SetStringProp(IMP_FBX_PASSWORD,      lString);
        manager->GetIOSettings()->SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

        lStatus = lImporter->Import(scene);

        if(lStatus == false && lImporter->GetLastErrorID() == FbxIOBase::ePasswordError)
        {
            FBXSDK_printf("\nPassword is wrong, import aborted.\n");
        }
    }
*/

    return lStatus;
}

void LoadScene(FbxScene& fbx_scene, class Scene& scene, Platform& platform)
{
    FbxNode* node = fbx_scene.GetRootNode();

    if(node)
    {
        for(Int32 i = 0; i < node->GetChildCount(); i++)
        {
			LoadAsset(*(node->GetChild(i)), fbx_scene, scene, platform);
        }
    }
}

void LoadAsset(FbxNode& node, FbxScene& fbx_scene, class Scene& scene, Platform& platform)
{
    FbxNodeAttribute::EType lAttributeType;

	const char* name = node.GetName();

    if(node.GetNodeAttribute() != NULL)
    {
        lAttributeType = node.GetNodeAttribute()->GetAttributeType();
        switch (lAttributeType)
        {
		case FbxNodeAttribute::eNull:
			for(Int32 i = 0; i < node.GetChildCount(); i++)
			{
				LoadAsset(*(node.GetChild(i)), fbx_scene, scene, platform);
			}
			break;
        case FbxNodeAttribute::eMarker:  
//            DisplayMarker(pNode);
            break;

        case FbxNodeAttribute::eSkeleton:
			{
				Skeleton* skeleton = new Skeleton();
				std::vector<JointPose> local_joint_transforms;

				AddSkeleton(node, fbx_scene, scene, skeleton, local_joint_transforms);
//            DisplaySkeleton(pNode);
			}
            break;

        case FbxNodeAttribute::eMesh:     
			{
				AddMesh(node, scene, platform);
			}
            break;

        case FbxNodeAttribute::eNurbs:      
//            DisplayNurb(pNode);
            break;

        case FbxNodeAttribute::ePatch:     
//            DisplayPatch(pNode);
            break;

        case FbxNodeAttribute::eCamera:    
//            DisplayCamera(pNode);
            break;

        case FbxNodeAttribute::eLight:     
//            DisplayLight(pNode);
            break;

        case FbxNodeAttribute::eLODGroup:
//            DisplayLodGroup(pNode);
            break;
        }   
	}
}

void AddMesh(FbxNode& node, Scene& scene, Platform& platform)
{
	// used for adding a mesh

	// material name -> triangles that use that material
	std::map<std::string, std::vector<MeshTriangle>> materials_primitives_;

	scene.meshes.push_back(MeshData());
	gef::MeshData& mesh = scene.meshes.back();
	gef::VertexData& vertex_buffer_data = mesh.vertex_data;


    FbxMesh* fbx_mesh = (FbxMesh*) node.GetNodeAttribute ();

	// Triangulate the mesh.
	if( !fbx_mesh->IsTriangleMesh() )
	{
		FbxGeometryConverter converter(sdk_manager);
		bool res = converter.Triangulate(fbx_mesh, true);

		fbx_mesh = node.GetMesh();
	}


	gef::StringId mesh_name_id = scene.string_id_table.Add(node.GetName());
	mesh.name_id = mesh_name_id;

    Int32 control_points_count = fbx_mesh->GetControlPointsCount();
	std::vector<ControlPoint> cp_data;

	// need to record min and max position values for mesh bounds
	gef::Vector4 pos_min(FLT_MAX, FLT_MAX, FLT_MAX), pos_max(-FLT_MAX, -FLT_MAX, -FLT_MAX);


	for (int cp_num = 0; cp_num < control_points_count; ++cp_num)
	{
		ControlPoint cp;
		cp.position;

		FbxVector4 position = fbx_mesh->GetControlPointAt(cp_num);
		position = fbx_axis_conversion_matrix_.MultT(position);

		cp.position.set_x(position[0] * fbx_scaling_factor_);
		cp.position.set_y(position[1] * fbx_scaling_factor_);
		cp.position.set_z(position[2] * fbx_scaling_factor_);

		// update min and max positions for bounds
		if (cp.position.x() < pos_min.x())
			pos_min.set_x(cp.position.x());
		if (cp.position.y() < pos_min.y())
			pos_min.set_y(cp.position.y());
		if (cp.position.z() < pos_min.z())
			pos_min.set_z(cp.position.z());
		if (cp.position.x() > pos_max.x())
			pos_max.set_x(cp.position.x());
		if (cp.position.y() > pos_max.y())
			pos_max.set_y(cp.position.y());
		if (cp.position.z() > pos_max.z())
			pos_max.set_z(cp.position.z());

		cp_data.push_back(cp);
	}

	// set mesh data bounds
	mesh.aabb = gef::Aabb(pos_min, pos_max);

	std::vector<PolyVertData> vb_data;

	// map from poly verts to vb verts
	std::map<int, int> polyvert_data;



	Int32 uv_count = GetMeshVertexUVCount(fbx_mesh);
	bool mesh_uvs_exist = uv_count > 0;
	Int32 skin_count =fbx_mesh->GetDeformerCount(FbxDeformer::eSkin);


	std::vector<std::list<Int32>> control_point_vertices;
	control_point_vertices.resize(control_points_count);

	// polygon vertex data
	Int32 poly_count = fbx_mesh->GetPolygonCount();
	Int32 mesh_vertex_index = 0;
	for(Int32 poly_index=0;poly_index<poly_count;++poly_index)
	{
		Int32 poly_vertex_count = fbx_mesh->GetPolygonSize(poly_index);
		MeshTriangle mesh_triangle;
		for(Int32 poly_vertex_index=0;poly_vertex_index<poly_vertex_count;++poly_vertex_index, ++mesh_vertex_index)
		{
			Int32 control_point_index = fbx_mesh->GetPolygonVertex(poly_index, poly_vertex_index);

			// Get UV
			Vector2 uv(0.0f, 0.0f);
			if (mesh_uvs_exist)
			{
				int uv_index = GetMeshVertexUVIndex(fbx_mesh, poly_index, poly_vertex_index, control_point_index);
				GetMeshVertexUV(uv, fbx_mesh, poly_index, poly_vertex_index, control_point_index);
				uv.y = -uv.y;
			}

			// Get Normal
			Vector4 normal(0.0f, 0.0f, 0.0f);
			GetMeshVertexNormal(normal, fbx_mesh, control_point_index, mesh_vertex_index);

			// add the UV and Normal to this control point
			Int32 cp_vertex_index;
			bool new_vertex = cp_data[control_point_index].AddVertexData(normal, uv, cp_vertex_index);

			// check to see if we have added a new vertex
			// if we have, add it to the vertex buffer data
			// if not, get the index into the vertex buffer data
			int vb_vertex_index = 0;
			if (new_vertex)
			{
				vb_vertex_index = vb_data.size();
				PolyVertData vert;
				vert.control_point_index = control_point_index;
				vert.vertex_index = cp_vertex_index;
				vb_data.push_back(vert);

				cp_data[control_point_index].SetVBVertexIndex(cp_vertex_index, vb_vertex_index);
			}
			else
			{
				vb_vertex_index = cp_data[control_point_index].GetVBVertexIndex(cp_vertex_index);
			}

			// this mapping from control points to the first vb vertex that uses it is only used for skinned vertices
			{
				// GRC FIXME? - assuming a one to one relationship between control points and vb verts
				// for skinned meshes
				control_point_vertices[control_point_index].push_back(vb_vertex_index);
			}

			if(poly_vertex_index < 3)
				mesh_triangle.index[2 - poly_vertex_index] = vb_vertex_index;
		}

		const char* material_name = GetMeshMaterial(fbx_mesh, poly_index, scene);
		materials_primitives_[material_name].push_back(mesh_triangle);
	}



	// build the vertex buffer
	{
		UInt32 vertex_buffer_size = vb_data.size();

		// allocate the vertex buffer
		void* vertices = NULL;
		if (skin_count > 0 && !fbx_ignore_skinning_)
		{
			vertices = (Mesh::SkinnedVertex*)malloc(vertex_buffer_size*sizeof(gef::Mesh::SkinnedVertex));
			vertex_buffer_data.vertex_byte_size = sizeof(gef::Mesh::SkinnedVertex);
		}
		else
		{
			vertices = (Mesh::Vertex*)malloc(vertex_buffer_size*sizeof(gef::Mesh::Vertex));
			vertex_buffer_data.vertex_byte_size = sizeof(gef::Mesh::Vertex);
		}
		memset(vertices, vertex_buffer_data.vertex_byte_size*vertex_buffer_size, 0xff);

		std::vector<bool> vertex_set(vertex_buffer_size, false);
		
		UInt8* vertex_data = (UInt8*)vertices;
		for (auto vertex = vb_data.begin(); vertex != vb_data.end(); ++vertex, vertex_data += vertex_buffer_data.vertex_byte_size)
		{
			const ControlPoint& cp = cp_data[vertex->control_point_index];
			const MeshVertexData& mesh_vertex_data = cp.vertex_data[vertex->vertex_index];


			if(skin_count > 0 && !fbx_ignore_skinning_)
			{
				Mesh::SkinnedVertex* skinned_vertex = (Mesh::SkinnedVertex*)vertex_data;
				skinned_vertex->position = cp.position;
				skinned_vertex->normal = cp.normals[mesh_vertex_data.normal_index];
				skinned_vertex->uv = cp.uvs[mesh_vertex_data.uv_index];
				skinned_vertex->bone_weights[0] = 1.0f;
				skinned_vertex->bone_weights[1] = 0.0f;
				skinned_vertex->bone_weights[2] = 0.0f;
				skinned_vertex->bone_weights[3] = 0.0f;
				skinned_vertex->bone_indices[0] = 0;
				skinned_vertex->bone_indices[1] = 0;
				skinned_vertex->bone_indices[2] = 0;
				skinned_vertex->bone_indices[3] = 0;
			}
			else
			{
				Mesh::Vertex* mesh_vertex = (Mesh::Vertex*)vertex_data;
				//mesh_vertex->position = gef::Vector4(cp.position, 1.0f);
				//mesh_vertex->normal = gef::Vector4(cp.normals[mesh_vertex_data.normal_index], 0.0f);
				//mesh_vertex->uv = cp.uvs[mesh_vertex_data.uv_index];

				mesh_vertex->px = cp.position.x();
				mesh_vertex->py = cp.position.y();
				mesh_vertex->pz = cp.position.z();
				mesh_vertex->nx = cp.normals[mesh_vertex_data.normal_index].x();
				mesh_vertex->ny = cp.normals[mesh_vertex_data.normal_index].y();
				mesh_vertex->nz = cp.normals[mesh_vertex_data.normal_index].z();
				mesh_vertex->u = cp.uvs[mesh_vertex_data.uv_index].x;
				mesh_vertex->v = cp.uvs[mesh_vertex_data.uv_index].y;
			}
		}
		vertex_buffer_data.vertices = vertices;
		vertex_buffer_data.num_vertices = vertex_buffer_size;
	}



//	mesh.bounding_sphere = gef::Sphere(mesh.aabb);

	
	UInt32 index_byte_size;
/*
	if(vertex_buffer_size <= 256)
		index_byte_size = 1;
	else if(vertex_buffer_size <= 65536)
		index_byte_size = 2;
	else
*/
	index_byte_size = 4;

	UInt32 primitive_index=0;
	for(std::map<std::string, std::vector<MeshTriangle>>::iterator primitiveIter = materials_primitives_.begin(); primitiveIter != materials_primitives_.end(); ++primitiveIter, ++primitive_index)
	{
		UInt32 triangle_count  = primitiveIter->second.size();
		UInt32 index_count = triangle_count*3;

		UInt8* indices = new UInt8[index_count*index_byte_size];

		for(UInt32 triangle_index=0;triangle_index<triangle_count;++triangle_index)
		{
			for(UInt32 index=0;index<3;++index)
			{
				UInt32 vertex_index = triangle_index*3+index;
				switch(index_byte_size)
				{
				case 1:
					indices[vertex_index] = (UInt8)primitiveIter->second[triangle_index].index[index];
					break;
				case 2:
					*((UInt16*)(&indices[vertex_index*index_byte_size])) = (UInt16)primitiveIter->second[triangle_index].index[index];
					break;
				default:
					*((UInt32*)(&indices[vertex_index*index_byte_size])) = (UInt32)primitiveIter->second[triangle_index].index[index];
					break;
				}
			}
		}

		PrimitiveData* primitive_data = new PrimitiveData();
		primitive_data->indices = indices;
		primitive_data->num_indices = index_count;
		primitive_data->index_byte_size = index_byte_size;
		primitive_data->type = TRIANGLE_LIST;
		mesh.primitives.push_back(primitive_data);


		gef::StringId material_name_id = gef::GetStringId(primitiveIter->first);
		primitive_data->material_name_id = material_name_id;
//		if(scene.material_data_map.find(material_name_id) != scene.material_data_map.end())
//			primitive_data->material = scene.material_data_map[material_name_id];
	}


	// clear out containers used to generate the mesh
	materials_primitives_.clear();

	if(!fbx_ignore_skinning_)
		AddMeshConnections(fbx_mesh, control_point_vertices, mesh, scene);
//	AddBlendShapes(fbx_mesh, *mesh, scene);
}




UInt32 GetMeshVertexCount(FbxMesh* fbx_mesh)
{
	Int32 poly_count = fbx_mesh->GetPolygonCount();
	UInt32 vertex_count = 0;
	for(Int32 poly_index=0;poly_index<poly_count;++poly_index)
		vertex_count += fbx_mesh->GetPolygonSize(poly_index);

	return vertex_count;
}


Vector4 GetMeshVertexPosition(const FbxMesh* const mesh, const Int32 control_point_index)
{
    FbxVector4* control_points = mesh->GetControlPoints();

	return Vector4(
		(float)control_points[control_point_index][0],
		(float)control_points[control_point_index][1],
		(float)control_points[control_point_index][2]
		);
}

bool GetMeshVertexNormal(Vector4& normal, FbxMesh* const fbx_mesh, const Int32 control_point_index, const Int32 vertex_index)
{
	bool normal_set = false;
	FbxVector4 fbx_normal(0.0f, 0.0f, 0.0f);

	//for( l = 0; l < pMesh->GetElementNormalCount(); ++l)
	int normal_count = fbx_mesh->GetElementNormalCount();
	if(fbx_mesh->GetElementNormalCount() > 0)
	{
		Int32 element_index = 0;
		const FbxGeometryElementNormal* fbx_normal_element = fbx_mesh->GetElementNormal(element_index);

		FbxGeometryElement::EMappingMode mapping_mode = fbx_normal_element->GetMappingMode();
		switch (mapping_mode)
		{
		case FbxGeometryElement::eByControlPoint:
			switch (fbx_normal_element->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				fbx_normal = fbx_normal_element->GetDirectArray().GetAt(control_point_index);
				normal_set = true;
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					Int32 id = fbx_normal_element->GetIndexArray().GetAt(control_point_index);
					fbx_normal = fbx_normal_element->GetDirectArray().GetAt(id);
					normal_set = true;
				}
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			{
				switch (fbx_normal_element->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					 fbx_normal = fbx_normal_element->GetDirectArray().GetAt(vertex_index);
					 normal_set = true;
					 //std::cout << control_point_index << ": " << fbx_normal[0] << " " << fbx_normal[1] << " " << fbx_normal[2] << std::endl;
					 //gef::DebugOut("%d: %f, %f, %f\n", control_point_index, (float)fbx_normal[0], (float)fbx_normal[1], (float)fbx_normal[2]);
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = fbx_normal_element->GetIndexArray().GetAt(vertex_index);
						fbx_normal = fbx_normal_element->GetDirectArray().GetAt(id);
						normal_set = true;
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
			}
			break;
		default:
			{
				float x = 5;
			}
			break;
		}
	}

	if(normal_set)
	{
		normal.set_x((float)fbx_normal[0]);
		normal.set_y((float)fbx_normal[1]);
		normal.set_z((float)fbx_normal[2]);
	}

	return normal_set;
}

bool MeshUVsExist(FbxMesh* const fbx_mesh)
{
	return (fbx_mesh->GetElementUVCount() > 0);
}

Int32 GetMeshVertexUVCount(FbxMesh* const fbx_mesh)
{
	Int32 uv_count = 0;


	if(fbx_mesh->GetElementUVCount() > 0)
	{
		Int32 element_index = 0;

		const FbxGeometryElementUV* fbx_uv_element = fbx_mesh->GetElementUV(element_index);

		switch (fbx_uv_element->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
		case FbxGeometryElement::eByPolygonVertex:
			uv_count = fbx_uv_element->GetDirectArray().GetCount();
			break;

		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			break;
		}
	}

	Int32 textureUVCount = fbx_mesh->GetTextureUVCount();
	if(uv_count != textureUVCount)
	{
		uv_count = 0;
	}

	return uv_count;
}


bool GetMeshVertexUV(Vector2& uv, FbxMesh* const fbx_mesh, const Int32 poly_index, const Int32 poly_vert_index, const Int32 control_point_index)
{
	bool uv_set = false;
	FbxVector2 fbx_uv;

//	for (l = 0; l < pMesh->GetElementUVCount(); ++l)
	if(fbx_mesh->GetElementUVCount() > 0)
	{
		Int32 element_index = 0;

		const FbxGeometryElementUV* fbx_uv_element = fbx_mesh->GetElementUV(element_index);

		switch (fbx_uv_element->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (fbx_uv_element->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				fbx_uv = fbx_uv_element->GetDirectArray().GetAt(control_point_index);
				uv_set = true;
				break;
			case FbxGeometryElement::eIndexToDirect:
				{
					Int32 id = fbx_uv_element->GetIndexArray().GetAt(control_point_index);
					fbx_uv = fbx_uv_element->GetDirectArray().GetAt(id);
					uv_set = true;
				}
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			{
				Int32 uv_index = fbx_mesh->GetTextureUVIndex(poly_index, poly_vert_index);
				switch (fbx_uv_element->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
				case FbxGeometryElement::eIndexToDirect:
					{
						fbx_uv =  fbx_uv_element->GetDirectArray().GetAt(uv_index);
						uv_set = true;
					}
					break;
				default:
					break; // other reference modes not shown here!
				}
			}
			break;

		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			break;
		}
	}

	if(uv_set)
	{
		uv.x = (float)fbx_uv[0];
		uv.y = (float)fbx_uv[1];
	}

	return uv_set;
}

Int32 GetMeshVertexUVIndex(FbxMesh* const fbx_mesh, const Int32 poly_index, const Int32 poly_vert_index, const Int32 control_point_index)
{
	Int32 uv_index = -1;

	if(fbx_mesh->GetElementUVCount() > 0)
	{
		Int32 element_index = 0;

		const FbxGeometryElementUV* fbx_uv_element = fbx_mesh->GetElementUV(element_index);

		switch (fbx_uv_element->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			switch (fbx_uv_element->GetReferenceMode())
			{
			case FbxGeometryElement::eDirect:
				uv_index = control_point_index;
				break;
			case FbxGeometryElement::eIndexToDirect:
				uv_index = fbx_uv_element->GetIndexArray().GetAt(control_point_index);
				break;
			default:
				break; // other reference modes not shown here!
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			uv_index = fbx_mesh->GetTextureUVIndex(poly_index, poly_vert_index);
			break;

		case FbxGeometryElement::eByPolygon: // doesn't make much sense for UVs
		case FbxGeometryElement::eAllSame:   // doesn't make much sense for UVs
		case FbxGeometryElement::eNone:       // doesn't make much sense for UVs
			break;
		}
	}

	return uv_index;
}


const char* GetMeshMaterial(FbxMesh* const fbx_mesh, const Int32 poly_index, gef::Scene& scene)
{
	const char* material_name = "";
    int lMtrlCount = 0;
    FbxNode* lNode = NULL;
    if(fbx_mesh){
        lNode = fbx_mesh->GetNode();
        if(lNode)
            lMtrlCount = lNode->GetMaterialCount();    
    }

    for (int l = 0; l < fbx_mesh->GetElementMaterialCount(); l++)
    {
        FbxGeometryElementMaterial* leMat = fbx_mesh->GetElementMaterial( l);
        if (leMat)
        {

			FbxSurfaceMaterial* lMaterial = fbx_mesh->GetNode()->GetMaterial(leMat->GetIndexArray().GetAt(poly_index));

			material_name = lMaterial->GetName();
			gef::StringId material_name_id = gef::GetStringId(material_name);

			std::map<gef::StringId, MaterialData*>::iterator material_iter = scene.material_data_map.find(material_name_id);
			if (material_iter == scene.material_data_map.end())
			{
				scene.string_id_table.Add(material_name);

				gef::MaterialData material_data;
				material_data.name_id = material_name_id;

				Int32 lMatId = leMat->GetIndexArray().GetAt(poly_index);

				if(lMatId >= 0)
				{
					FbxProperty diffuseProperty = lMaterial->FindProperty(FbxSurfaceMaterial::sDiffuse);
					int lNbTextures = diffuseProperty.GetSrcObjectCount<FbxTexture>();
					
					if(lNbTextures > 0)
					{
						for(int j =0; j<lNbTextures; ++j)
						{
							FbxTexture* lTexture = diffuseProperty.GetSrcObject<FbxTexture>(j);

							FbxFileTexture *lFileTexture = FbxCast<FbxFileTexture>(lTexture);

							if(lFileTexture)
							{
								const char* texture_filename = lFileTexture->GetRelativeFileName();


								material_data.diffuse_texture = texture_filename;

								// replace all \ with /
								for(std::string::iterator string_iter = material_data.diffuse_texture.begin();string_iter != material_data.diffuse_texture.end();++string_iter)
								{
									if(*string_iter == '\\')
										*string_iter = '/';
								}

								if(fbx_texture_filename_ext_ || fbx_strip_texture_path_)
								{
									char drive[_MAX_DRIVE];
									char dir[_MAX_DIR];
									char fname[_MAX_FNAME];
									char ext[_MAX_EXT];

									_splitpath(material_data.diffuse_texture.c_str(),
										drive,
										dir,
										fname,
										ext);

									if(fbx_strip_texture_path_)
									{
										material_data.diffuse_texture = fname;
										material_data.diffuse_texture += ext;
									}

									if(fbx_texture_filename_ext_)
									{
										material_data.diffuse_texture.erase(material_data.diffuse_texture.find(ext), strlen(ext));
										material_data.diffuse_texture += fbx_texture_filename_ext_;
									}
								}

								FBXSDK_printf("texture filename: %s\n", material_data.diffuse_texture.c_str());
							}
						}
					}
				}

				scene.material_data.push_back(material_data);
				scene.material_data_map[material_name_id] = &scene.material_data.back();
			}
		}
	}
	return material_name;
}

void AddMeshConnections(FbxGeometry* geometry, const std::vector<std::list<Int32>>& control_point_vertices, MeshData& mesh, Scene& scene)
{
    int i, j;
    int lSkinCount=0;
    int lClusterCount=0;
    FbxCluster* lCluster;

	std::vector<Int32> weight_indices;
	weight_indices.resize(control_point_vertices.size(), 0);


    lSkinCount=geometry->GetDeformerCount(FbxDeformer::eSkin);

	Int32 cluster_start_index = scene.skin_cluster_name_ids.size();

//	std::vector<Int32> normalise_vertices;

    for(i=0; i!=lSkinCount; ++i)
    {
//		SkinDeformer* skin_deformer = new SkinDeformer();

        lClusterCount = ((FbxSkin *) geometry->GetDeformer(i, FbxDeformer::eSkin))->GetClusterCount();

		for (j = 0; j != lClusterCount; ++j)
        {
            lCluster=((FbxSkin *) geometry->GetDeformer(i, FbxDeformer::eSkin))->GetCluster(j);

			const char* link_name = NULL;

//			SkinCluster skin_cluster;
            if(lCluster->GetLink() != NULL)
            {
				link_name = lCluster->GetLink()->GetName();
				StringId link_name_id = GetStringId(link_name);
//				skin_cluster.link_name = link_name_id;
				scene.skin_cluster_name_ids.push_back(link_name_id);
            }
			else
			{
				scene.skin_cluster_name_ids.push_back(0);
			}

            int k, lIndexCount = lCluster->GetControlPointIndicesCount();
            int* lIndices = lCluster->GetControlPointIndices();
            double* lWeights = lCluster->GetControlPointWeights();

			for(k = 0; k < lIndexCount; k++)
            {
				Int32 control_point_index = lIndices[k];
				float weight = (float)lWeights[k];
				Int32 cluster_index = cluster_start_index+j;
				Int32 influence_index = weight_indices[control_point_index];
				assert(cluster_index < 255);

				// go through all vertices that use this control point and set the bone indices and weights
				// indices use cluster index at the moment. These must be matched to skeleton after the scene is loaded
				for(std::list<Int32>::const_iterator vertex_iter = control_point_vertices[control_point_index].begin(); vertex_iter != control_point_vertices[control_point_index].end(); ++vertex_iter)
				{
					Int32 vertex_index = (*vertex_iter);
					Mesh::SkinnedVertex* vertex =  (Mesh::SkinnedVertex*)((UInt8*)mesh.vertex_data.vertices+vertex_index*mesh.vertex_data.vertex_byte_size);


					// if the influence index is greater 3 then replace the influence with the smallest weight
					if(influence_index > 3)
					{
						Int32 smallest_index = 0;
						float smallest_weight = vertex->bone_weights[0];

						for(Int32 i=1;i<4;++i)
						{
							if(vertex->bone_weights[i] < smallest_weight)
							{
								smallest_weight = vertex->bone_weights[i];
								smallest_index = i;
							}
						}

						// if the current weight is greater than the smallest weight the discard the smallest weight
						// and replace with the current weight
						if(weight > smallest_weight)
						{
							vertex->bone_indices[smallest_index] = cluster_index;
							vertex->bone_weights[smallest_index] = weight;

							// we need to normalise the weights for this vertex
							//normalise_vertices.push_back(*vertex_iter);
						}
					}
					else
					{
						vertex->bone_indices[influence_index] = cluster_index;
						vertex->bone_weights[influence_index] = weight;
					}
				}

				if(influence_index < 4)
					weight_indices[control_point_index]++;

            }

			//FbxAMatrix lMatrix;
            //lMatrix = lCluster->GetTransformMatrix(lMatrix);
            //if (lCluster->GetAssociateModel() != NULL)
            //{
            //    lMatrix = lCluster->GetTransformAssociateModelMatrix(lMatrix);
            //}
		} // cluster count
//		mesh.AddDeformer(skin_deformer);
    } // skin count


	// go through all control points that need normalised
/*
	std::sort(normalise_vertices.begin(), normalise_vertices.end());
	Int32 current_vertex = -1;
	float weight_total = 1.0f;
	for(std::vector<Int32>::iterator vertex_iter = normalise_vertices.begin(); vertex_iter != normalise_vertices.end(); ++vertex_iter)
	{
		if(*vertex_iter != current_vertex)
		{
			current_vertex = *vertex_iter;

			Mesh::SkinnedVertex* vertex =  (Mesh::SkinnedVertex*)((UInt32)mesh.vertex_data.vertices+current_vertex*mesh.vertex_data.vertex_byte_size);

			weight_total = vertex->bone_weights[0]+vertex->bone_weights[1]+vertex->bone_weights[2]+vertex->bone_weights[3];

			vertex->bone_weights[0] /= weight_total;
			vertex->bone_weights[1] /= weight_total;
			vertex->bone_weights[2] /= weight_total;
			vertex->bone_weights[3] /= weight_total;
		}
	}
*/
}

void AddSkeleton(FbxNode& node, FbxScene& fbx_scene, Scene& scene, Skeleton* skeleton, std::vector<JointPose>& local_joint_transforms, FbxNode* parent_node)
{
	const char* node_name = node.GetName();


	// add this joint to the skeleton
//    FbxAnimEvaluator* lEvaluator = fbx_scene.GetEvaluator();
//	FbxAMatrix fbx_local_matrix = lEvaluator->GetNodeLocalTransform(&node);
	FbxAMatrix fbx_local_matrix = node.EvaluateLocalTransform();
	FbxVector4 fbx_scale = fbx_local_matrix.GetS();
	FbxVector4 fbx_translation = fbx_local_matrix.GetT();

	JointPose joint_pose;
	joint_pose.set_translation(Vector4((float)fbx_translation[0]*fbx_scaling_factor_, (float)fbx_translation[1]*fbx_scaling_factor_, (float)fbx_translation[2]*fbx_scaling_factor_));
//	joint_pose.set_scale(Vector4((float)fbx_scale[0], (float)fbx_scale[1], (float)fbx_scale[2]));


	// assuming scale of 1.f for skeleton joint
	joint_pose.set_scale(Vector4(1.0f, 1.0f, 1.0f));

	// get rotation as a quaternion
	FbxVector4 joint_orient = node.GetPreRotation(FbxNode::eSourcePivot);
	FbxAMatrix joint_orient_matrix;
	FbxRotationOrder& rotationOrder = node.GetTransform().GetRotationOrder();
	rotationOrder.V2M(joint_orient_matrix, FbxVector4(joint_orient[0], joint_orient[1], joint_orient[2]));
	FbxQuaternion fbx_rotationQ = joint_orient_matrix.GetQ();


	//FbxQuaternion fbx_rotationQ = fbx_local_matrix.GetUnnormalizedQ();
	fbx_rotationQ.Normalize();

	joint_pose.set_rotation(Quaternion((float)fbx_rotationQ.GetAt(0), (float)fbx_rotationQ.GetAt(1), (float)fbx_rotationQ.GetAt(2), (float)fbx_rotationQ.GetAt(3)));
	local_joint_transforms.push_back(joint_pose);

	// populate the joint name and parent id
	// inverse bind pose matrix will be calculated once the skeleton is complete
	Joint joint;
	joint.name_id = scene.string_id_table.Add(node_name);
	if(parent_node)
		joint.parent = skeleton->FindJointIndex(GetStringId(parent_node->GetName()));
	else
		joint.parent = -1;
	Int32 joint_index = skeleton->AddJoint(joint);

//	char debug_string[256];
//	sprintf(debug_string, "%d: %s 0x%x\n", joint_index, node_name, joint.name);
//	OutputDebugStringA(debug_string);

	// go through children and add them to the current skeleton
	for(Int32 child_index = 0; child_index<node.GetChildCount();++child_index)
	{
		FbxNode* child_node = node.GetChild(child_index);
		if(node.GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
			AddSkeleton(*child_node, fbx_scene, scene, skeleton, local_joint_transforms, &node);
	}

	// if this is the root joint then building the skeleton, add it to the scene
	if(parent_node == NULL)
	{
		// calculate global joint transforms
		std::vector<Matrix44> global_joint_transforms;

		for(Int32 joint_index=0;joint_index<skeleton->joint_count();++joint_index)
		{
			Joint& joint = skeleton->joint(joint_index);

			std::string joint_name;
			scene.string_id_table.Find(joint.name_id, joint_name);
			Matrix44 local_joint_transform = local_joint_transforms[joint_index].GetMatrix();
			Matrix44 global_joint_transform;
			if(joint.parent == -1)
				global_joint_transform = local_joint_transform;
			else
				global_joint_transform = local_joint_transform * global_joint_transforms[joint.parent];
			global_joint_transforms.push_back(global_joint_transform);

			joint.inv_bind_pose.AffineInverse(global_joint_transform);
		}

		local_joint_transforms.clear();
		scene.skeletons.push_back(skeleton);
	}
}

void AddAnimation(FbxScene& fbx_scene, class Scene& scene)
{

    int lAnimStackCount = lImporter->GetAnimStackCount();

    for (Int32 stack_index = 0; stack_index < lAnimStackCount; stack_index++)
//	if(fbx_scene.GetSrcObjectCount<FbxAnimStack>() > 0)
    {
//		Int32 stack_index = 0;

		FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(stack_index);


		const char* take_name = lTakeInfo->mName.Buffer();

        FbxAnimStack* anim_stack = fbx_scene.GetSrcObject<FbxAnimStack>(stack_index);

		Animation* anim = new Animation();

		gef::StringId anim_name_id = scene.string_id_table.Add(take_name);
		anim->set_name_id(anim_name_id);

		FbxTime start_time = lTakeInfo->mLocalTimeSpan.GetStart();
		FbxTime end_time = lTakeInfo->mLocalTimeSpan.GetStop();

		anim->set_start_time(start_time.GetMilliSeconds() / 1000.f);
		anim->set_end_time(end_time.GetMilliSeconds() / 1000.f);
		anim->CalculateDuration();
		scene.animations[anim->name_id()] = anim;


        AddAnimation(*fbx_scene.GetRootNode(), *anim_stack, *anim);
    }

}

void AddAnimation(FbxNode& fbx_node, FbxAnimStack& fbx_anim_stack, class Animation& anim)
{
    int layer_count = fbx_anim_stack.GetMemberCount<FbxAnimLayer>();
    FbxString lOutputString;

	if(layer_count > 1)
		layer_count = 1;

	for (Int32 layer_index = 0; layer_index < layer_count; ++layer_index)
    {
        FbxAnimLayer* anim_layer = fbx_anim_stack.GetMember<FbxAnimLayer>(layer_index);

		const char* layer_name = anim_layer->GetName();



        AddAnimation(fbx_node, *anim_layer, anim);
    }
}

template <typename T> void AddKeyTimes(std::vector<T>& keys, FbxAnimCurve* anim_curve)
{
	if(anim_curve)
	{
		Int32 key_count = anim_curve->KeyGetCount();

		std::vector<T>::iterator key_iter = keys.begin();

		for(Int32 key_index = 0; key_index < key_count; ++key_index)
		{
			FbxTime time = anim_curve->KeyGetTime(key_index);
//			float time_in_secs = (float)time.GetMilliSeconds() / 1000.f;

			float time_in_secs = time.GetFrameCountPrecise(FbxTime::eFrames60) / 60.0f;
		
			for(;key_iter != keys.end(); ++key_iter)
			{
				if(key_iter->time >= time_in_secs)
					break;
			}

			T key;
			key.time = time_in_secs;
			if(key_iter == keys.end())
			{
				// all the existing keys are at times before the key time from the animation curve
				// add the key to the end
				keys.push_back(key);

				// update the iterator to be at the end of the updated keys container
				key_iter = keys.end();
			}
			else
			{
				// check to see if there is already a key at this time
				if(key_iter->time != time_in_secs)
				{
					// insert a key before this one
					key_iter = keys.insert(key_iter, key);
				}
			}
		}
	}
}


void AddAnimation(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer, class Animation& anim)
{
	const char* node_name = fbx_node.GetName();

	gef::Quaternion joint_orientQ(0.0f, 0.0f, 0.0f, 1.0f);

	if(fbx_node.GetNodeAttribute() && fbx_node.GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		FbxVector4 joint_rotation = fbx_node.GetPreRotation(FbxNode::eSourcePivot);

		FbxRotationOrder& rotationOrder = fbx_node.GetTransform().GetRotationOrder();
		FbxAMatrix fbx_joint_orient_matrix;
		rotationOrder.V2M(fbx_joint_orient_matrix, FbxVector4(joint_rotation[0], joint_rotation[1], joint_rotation[2]));
		FbxQuaternion fbx_joint_orientQ = fbx_joint_orient_matrix.GetQ();

		joint_orientQ = Quaternion((float)fbx_joint_orientQ.GetAt(0), (float)fbx_joint_orientQ.GetAt(1), (float)fbx_joint_orientQ.GetAt(2), (float)fbx_joint_orientQ.GetAt(3));
	}


	if(TranformKeysExist(fbx_node, fbx_anim_layer))
	{
		TransformAnimNode* anim_node = new TransformAnimNode();
		const char* name = fbx_node.GetName();
		anim_node->set_name_id(GetStringId(name));
		if(ScaleKeysExist(fbx_node, fbx_anim_layer))
		{
			// Add scale keys at all the different times that keys appear on scale animation curves
			AddKeyTimes<Vector3Key>(anim_node->scale_keys(), fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X));
			AddKeyTimes<Vector3Key>(anim_node->scale_keys(), fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y));
			AddKeyTimes<Vector3Key>(anim_node->scale_keys(), fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z));

			// go through all keys and set values
			for(std::vector<Vector3Key>::iterator key_iter = anim_node->scale_keys().begin(); key_iter != anim_node->scale_keys().end();++key_iter)
				key_iter->value = GetVector3AnimCurveValue(key_iter->time, fbx_node.LclScaling, fbx_anim_layer);
		}
		if(RotationKeysExist(fbx_node, fbx_anim_layer))
		{
			// rotation
			// Add rotation keys at all the different times that keys appear on rotation animation curves
			AddKeyTimes<QuaternionKey>(anim_node->rotation_keys(), fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X));
			AddKeyTimes<QuaternionKey>(anim_node->rotation_keys(), fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y));
			AddKeyTimes<QuaternionKey>(anim_node->rotation_keys(), fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z));

			for(std::vector<QuaternionKey>::iterator key_iter = anim_node->rotation_keys().begin(); key_iter != anim_node->rotation_keys().end();++key_iter)
			{
				Vector4 euler_rotation = GetVector3AnimCurveValue(key_iter->time, fbx_node.LclRotation, fbx_anim_layer);

			    FbxVector4 joint_rotation = fbx_node.GetPreRotation(FbxNode::eSourcePivot);


				FbxRotationOrder& rotationOrder = fbx_node.GetTransform().GetRotationOrder();
				FbxAMatrix rotation;
				rotationOrder.V2M(rotation, FbxVector4(euler_rotation.x(), euler_rotation.y(), euler_rotation.z()));
				FbxQuaternion fbx_quaternion = rotation.GetQ();

				key_iter->value = 
					Quaternion((float)fbx_quaternion[0], (float)fbx_quaternion[1], (float)fbx_quaternion[2], (float)fbx_quaternion[3])
					*
					joint_orientQ
					;


			}
		}
		if(TranslationKeysExist(fbx_node, fbx_anim_layer))
		{
			// translation
			// Add translation keys at all throtatione different times that keys appear on translation animation curves
			AddKeyTimes<Vector3Key>(anim_node->translation_keys(), fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X));
			AddKeyTimes<Vector3Key>(anim_node->translation_keys(), fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y));
			AddKeyTimes<Vector3Key>(anim_node->translation_keys(), fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z));

			// go through all keys and set values
			for(std::vector<Vector3Key>::iterator key_iter = anim_node->translation_keys().begin(); key_iter != anim_node->translation_keys().end();++key_iter)
			{
				key_iter->value = GetVector3AnimCurveValue(key_iter->time, fbx_node.LclTranslation, fbx_anim_layer);
				key_iter->value *= fbx_scaling_factor_;

				//gef::DebugOut("%f: %f %f %f\n", key_iter->time, key_iter->value.x, key_iter->value.y, key_iter->value.z);
			}
		}
		anim.AddNode(anim_node);
	}
/*
	else if(BlendShapeChannelKeysExist(fbx_node, fbx_anim_layer))
	{
		AddBlendShapeChannelKeys(fbx_node, fbx_anim_layer, anim);
	}
*/
	for(Int32 i = 0; i < fbx_node.GetChildCount(); i++)
	{
		AddAnimation(*(fbx_node.GetChild(i)), fbx_anim_layer, anim);
	}

}

bool TranformKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer)
{
	return ScaleKeysExist(fbx_node, fbx_anim_layer) || RotationKeysExist(fbx_node, fbx_anim_layer) || TranslationKeysExist(fbx_node, fbx_anim_layer);
}

bool ScaleKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer)
{
	return
		(fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X) != NULL) || 
		(fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y) != NULL) ||
		(fbx_node.LclScaling.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z) != NULL);
}

bool RotationKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer)
{
	return
		(fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X) != NULL) || 
		(fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y) != NULL) ||
		(fbx_node.LclRotation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z) != NULL);
}

bool TranslationKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer)
{
	return
		(fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X) != NULL) || 
		(fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y) != NULL) ||
		(fbx_node.LclTranslation.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z) != NULL);
}

/*
bool BlendShapeChannelKeysExist(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer)
{
	bool keys_found = false;

	FbxNodeAttribute* attribute = fbx_node.GetNodeAttribute();

	if(attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* fbx_mesh = (FbxMesh*) fbx_node.GetNodeAttribute();

		Int32 blend_shape_count, lBlendShapeChannelCount;
		FbxBlendShape* lBlendShape;

		blend_shape_count = fbx_mesh->GetDeformerCount(FbxDeformer::eBlendShape);

		for (int lBlendShapeIndex = 0; lBlendShapeIndex < blend_shape_count; ++lBlendShapeIndex)
		{
			lBlendShape = (FbxBlendShape*) fbx_mesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);   
			lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			for(int lBlendShapeChannelIndex = 0;  lBlendShapeChannelIndex < lBlendShapeChannelCount; ++lBlendShapeChannelIndex)
			{
				FbxAnimCurve* lAnimCurve = fbx_mesh->GetShapeChannel(lBlendShapeIndex, lBlendShapeChannelIndex, &fbx_anim_layer, true);
				if(lAnimCurve)
				{
					keys_found = true;
					break;
				}
			}
		}
	}
	return keys_found;
}
*/

/*
void AddBlendShapeChannelKeys(FbxNode& fbx_node, FbxAnimLayer& fbx_anim_layer, class Animation& anim)
{
	FbxNodeAttribute* attribute = fbx_node.GetNodeAttribute();

	if(attribute && attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
	{
		FbxMesh* fbx_mesh = (FbxMesh*) fbx_node.GetNodeAttribute();

		Int32 blend_shape_count, lBlendShapeChannelCount;
		FbxBlendShape* lBlendShape;

		blend_shape_count = fbx_mesh->GetDeformerCount(FbxDeformer::eBlendShape);

		for (int lBlendShapeIndex = 0; lBlendShapeIndex < blend_shape_count; ++lBlendShapeIndex)
		{
			lBlendShape = (FbxBlendShape*) fbx_mesh->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);   
			lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
			for(int lBlendShapeChannelIndex = 0;  lBlendShapeChannelIndex < lBlendShapeChannelCount; ++lBlendShapeChannelIndex)
			{
				FbxAnimCurve* lAnimCurve = fbx_mesh->GetShapeChannel(lBlendShapeIndex, lBlendShapeChannelIndex, &fbx_anim_layer, true);
				if(lAnimCurve)
				{
					ChannelAnimNode* anim_node = new ChannelAnimNode();
					anim_node->set_name(GetStringId(lBlendShape->GetBlendShapeChannel(lBlendShapeChannelIndex)->GetName()));

					Int32 key_count = lAnimCurve->KeyGetCount();
					for(Int32 key_index = 0; key_index < key_count; ++key_index)
					{
						FbxTime time = lAnimCurve->KeyGetTime(key_index);
						float time_in_secs = (float)time.GetMilliSeconds() / 1000.f;
		
						ChannelKey key;
						key.time = time_in_secs;
						key.value  = lAnimCurve->KeyGetValue(key_index) * 0.01f;
						anim_node->keys().push_back(key);
					}

					// Animation class now owns anim_node
					anim.AddNode(anim_node);
				}
			}
		}
	}
}
*/


Vector4 GetVector3AnimCurveValue(const float time_in_secs, FbxPropertyT<FbxDouble3>& node_property, FbxAnimLayer& fbx_anim_layer)
{
	Vector4 result;
	FbxTime time;
//	time.SetMilliSeconds(FbxLongLong(time_in_secs*1000.0f));
	time.SetFramePrecise(time_in_secs*60.0f, FbxTime::eFrames60);

	FbxDouble3 default_value = node_property.Get();

	FbxAnimCurve* anim_curve_x = node_property.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_X);
	FbxAnimCurve* anim_curve_y = node_property.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Y);
	FbxAnimCurve* anim_curve_z = node_property.GetCurve(&fbx_anim_layer, FBXSDK_CURVENODE_COMPONENT_Z);
	if(anim_curve_x)
		result.set_x(anim_curve_x->Evaluate(time));
	else
		result.set_x((float)default_value[0]);

	if(anim_curve_y)
		result.set_y(anim_curve_y->Evaluate(time));
	else
		result.set_y((float)default_value[1]);

	if(anim_curve_z)
		result.set_z(anim_curve_z->Evaluate(time));
	else
		result.set_z((float)default_value[2]);

	return result;
}

/*
void AddBlendShapes(FbxGeometry* geometry, class Mesh& mesh, Scene& scene)
{
    Int32 blend_shape_count, lBlendShapeChannelCount, lTargetShapeCount;
    FbxBlendShape* lBlendShape;
	FbxBlendShapeChannel* lBlendShapeChannel;
	FbxShape* lShape;

    blend_shape_count = geometry->GetDeformerCount(FbxDeformer::eBlendShape);
	const char* mesh_name = geometry->GetName();

    for (int lBlendShapeIndex = 0; lBlendShapeIndex < blend_shape_count; ++lBlendShapeIndex)
    {
		lBlendShape = (FbxBlendShape*) geometry->GetDeformer(lBlendShapeIndex, FbxDeformer::eBlendShape);   
		const char* blend_shape_name = lBlendShape->GetName();
//        DisplayString("    BlendShape ", (char *) lBlendShape->GetName());

		BlendShape* blend_shape = new BlendShape();
		BlendShapeDeformer* blend_shape_deformer = new BlendShapeDeformer();
        
		lBlendShapeChannelCount = lBlendShape->GetBlendShapeChannelCount();
		blend_shape_deformer->SetChannelCount(lBlendShapeChannelCount);
		for(int lBlendShapeChannelIndex = 0;  lBlendShapeChannelIndex < lBlendShapeChannelCount; ++lBlendShapeChannelIndex)
		{
			BlendShapeDeformer::Channel& blend_shape_channel = blend_shape_deformer->GetChannel(lBlendShapeChannelIndex);
			lBlendShapeChannel = lBlendShape->GetBlendShapeChannel(lBlendShapeChannelIndex);
			const char* blend_shape_channel_name = lBlendShapeChannel->GetName();

			blend_shape_channel.name = GetStringId(blend_shape_channel_name);
			blend_shape_channel.default_value = static_cast<float>(lBlendShapeChannel->DeformPercent.Get());

			BlendShapeChannel channel;
			channel.name = GetStringId(blend_shape_channel_name);
			channel.value = static_cast<float>(lBlendShapeChannel->DeformPercent.Get());
			blend_shape->channels().push_back(channel);


			lTargetShapeCount = lBlendShapeChannel->GetTargetShapeCount();
			blend_shape_channel.targets.resize(lTargetShapeCount);
			for (int lTargetShapeIndex = 0; lTargetShapeIndex < lTargetShapeCount; ++lTargetShapeIndex)
			{
				BlendShapeDeformer::Target& target = blend_shape_channel.targets[lTargetShapeIndex];
				lShape = lBlendShapeChannel->GetTargetShape(lTargetShapeIndex);
				//DisplayString("    TargetShape ", (char *) lShape->GetName());
				const char* target_shape_name = lShape->GetName();

				int j, lControlPointsCount = lShape->GetControlPointsCount();
				FbxVector4* lControlPoints = lShape->GetControlPoints();
				FbxLayerElementArrayTemplate<FbxVector4>* lNormals = NULL;    
				bool lStatus = lShape->GetNormals(&lNormals);
				int lNormalCount = lNormals->GetCount();

				FbxVector4* lOriginalControlPoints =   lBlendShape->GetGeometry()->GetControlPoints();
				int lOriginalControlPointsCount = geometry->GetControlPointsCount();

				FbxLayerElementArrayTemplate<FbxVector4>* lOriginalNormals = NULL;    
				lStatus = lBlendShape->GetGeometry()->GetNormals(&lOriginalNormals);
				int lOriginalNormalCount = lOriginalNormals->GetCount();

				//const Vertex* mesh_vertex = (const Vertex*)mesh.vertices();

				for(j = 0; j < lControlPointsCount; j++)
				{
//					DisplayInt("        Control Point ", j);
//					Display3DVector("            Coordinates:1 ", lControlPoints[j]);
					FbxVector4 lControlPoint = lControlPoints[j];
					FbxVector4 lOriginalControlPoint = lOriginalControlPoints[j];
					//BlendShapeDeformer::TargetVertex vertex;
					Vector4 position_delta;
					position_delta.x = static_cast<float>(lControlPoint[0]) - static_cast<float>(lOriginalControlPoint[0]));
					position_delta.y = static_cast<float>(lControlPoint[1]) - static_cast<float>(lOriginalControlPoint[1]));
					position_delta.z = static_cast<float>(lControlPoint[2]) - static_cast<float>(lOriginalControlPoint[2]));
					target.position_deltas.push_back(position_delta);

				}

				if (lStatus && lNormals && lNormalCount == lOriginalNormalCount)
				{
					for(j = 0; j < lNormalCount; j++)
					{
						FbxVector4 lNormal = lNormals->GetAt(j);
						FbxVector4 lOriginalNormal = lOriginalNormals->GetAt(j);

//						char buffer[256];
//						sprintf(buffer, "%d: nx: %f ny: %f nz: %f\n%d: ox: %f oy: %f oz: %f\n", j, lNormal[0], lNormal[1], lNormal[2], j, lOriginalNormal[0], lOriginalNormal[1], lOriginalNormal[2]);
//						OutputDebugStringA(buffer);
						Vector4 normal_delta;
						normal_delta.x = static_cast<float>(lNormal[0]) - static_cast<float>(lOriginalNormal[0]));
						normal_delta.y = static_cast<float>(lNormal[1]) - static_cast<float>(lOriginalNormal[1]));
						normal_delta.z = static_cast<float>(lNormal[2]) - static_cast<float>(lOriginalNormal[2]));
						target.normal_deltas.push_back(normal_delta);
					}
				}
				//DisplayString("");
			}
		}
		scene.blend_shapes().push_back(blend_shape);

		// mesh now owns deformer
		mesh.AddDeformer(blend_shape_deformer);
    }
}
*/

#endif

}