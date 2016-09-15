#ifndef _GEF_MESH_INSTANCE_H
#define _GEF_MESH_INSTANCE_H

#include <maths/matrix44.h>

namespace gef
{
	class Mesh;

	/**
	An instance of an object the visually represented by a Mesh and is placed in the world by its own transform.
	*/
	class MeshInstance
	{
	public:
		/// @brief Default constructor.
		MeshInstance();

		/// @brief Get the transform
		/// @return The transformation matrix
		const Matrix44& transform() const {return transform_;}

		/// @brief Set the transform
		/// @param[in] transform	the transformation matrix
		void set_transform(const Matrix44& transform) { transform_ = transform; }

		/// @brief Get the mesh
		/// @return The mesh
		/// @note The pointer returned here prevents modifying the Mesh object. This is intentional. Mesh objects should be modifed via an individual MeshInstance
		/// as other MeshInstance objects may be refering to the same Mesh object. Allowing modification via this route could have unintentional knock ons.
		const class Mesh* mesh() const {return mesh_;}

		/// @brief Set the mesh
		/// @param[in] mesh		The mesh that visually represents this object
		void set_mesh(const Mesh* mesh) { mesh_ = mesh; }
	protected:
		/// The transformation matrix.
		Matrix44 transform_;

		/// The mesh
		const Mesh* mesh_;
	};
}

#endif // _GEF_MESH_INSTANCE_H
