#include <graphics/model.h>
#include <vector>
#include <maths/vector4.h>
#include <maths/vector2.h>
#include <graphics/mesh.h>
#include <graphics/primitive.h>
#include <system/platform.h>
#include <graphics/texture.h>


namespace gef
{
	Model::Model() :
		mesh_(NULL)
	{
	}

	Model::~Model()
	{
		Release();
	}

	void Model::Release()
	{
		for(UInt32 texture_num=0;texture_num<textures_.size();++texture_num)
			DeleteNull(textures_[texture_num]);
		textures_.clear();
		DeleteNull(mesh_);
	}
}