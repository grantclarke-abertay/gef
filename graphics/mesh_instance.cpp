#include <graphics/mesh_instance.h>
#include <cstddef>

namespace gef
{
	MeshInstance::MeshInstance() :
		mesh_(NULL)
	{
		transform_.SetIdentity();
	}
}
