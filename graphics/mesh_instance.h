#ifndef _GEF_MESH_INSTANCE_H
#define _GEF_MESH_INSTANCE_H

#include <maths/matrix44.h>

namespace gef
{
	class Mesh;

	class MeshInstance
	{
	public:
		MeshInstance();
		const Matrix44& transform() const {return transform_;}
		void set_transform(const Matrix44& transform) { transform_ = transform; }
		const class Mesh* mesh() const {return mesh_;}
		void set_mesh(const class Mesh* mesh) { mesh_ = mesh; }
	protected:
		Matrix44 transform_;
		const Mesh* mesh_;

	};
}

#endif // _GEF_MESH_INSTANCE_H
