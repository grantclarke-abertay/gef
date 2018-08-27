#ifndef _PRIMITIVE_BUILDER_H
#define _PRIMITIVE_BUILDER_H

#include <maths/vector4.h>
#include <graphics/material.h>
#include <cstddef>
//#include "load_texture.h"


namespace gef
{
	class Mesh;
	class Platform;
}

class PrimitiveBuilder
{
public:
	/// @brief Constructor.
	/// @param[in] platform		The platform the primitive builder is being created on.
	PrimitiveBuilder(gef::Platform& platform);

	/// @brief Default destructor.
	~PrimitiveBuilder();

	/// @brief Initialises the primitive builder.
	void Init();

	/// @brief Clean up resources created by the primitive builder.
	void CleanUp();

	/// @brief Creates a box shaped mesh
	/// @return The mesh created
	/// @param[in] half_size	The half size of the box.
	/// @param[in] centre		The centre of the box.
	/// @param[in] materials	an array of Material pointers. One for each face. 6 in total.
	gef::Mesh* CreateBoxMesh(const gef::Vector4& half_size, gef::Vector4 centre = gef::Vector4(0.0f, 0.0f, 0.0f), gef::Material** materials = NULL);


	/// @brief Creates a sphere shaped mesh
	/// @return The mesh created
	/// @param[in] radius		The radius of the sphere.
	/// @param[in] centre		The centre of the centre.
	/// @param[in] materials	Pointer to material used to render all faces. NULL is valid.
	gef::Mesh* CreateSphereMesh(const float radius, const int phi, const int theta, gef::Vector4 centre = gef::Vector4(0.0f, 0.0f, 0.0f), gef::Material* material = NULL);


	/// @brief Get the default cube mesh.
	/// @return The mesh for the default cube.
	/// @note The default cube has dimensions 1 x 1 x 1 with the centre at 0, 0, 0.
	inline const gef::Mesh* GetDefaultCubeMesh() const {
		return default_cube_mesh_;
	};

	/// @brief Get the default sphere mesh.
	/// @return The mesh for the default cube.
	/// @note The default sphere has radius 0.5 with the centre at 0, 0, 0.
	inline const gef::Mesh* GetDefaultSphereMesh() const {
		return default_sphere_mesh_;
	};


	/// @brief Get the red material.
	/// @return The reference to the red material.
	inline const gef::Material& red_material() const {
		return red_material_;
	}

	/// @brief Get the red material.
	/// @return The reference to the green material.
	inline const gef::Material& green_material() const {
		return green_material_;
	}

	/// @brief Get the blue material.
	/// @return The reference to the blue material.
	inline const gef::Material& blue_material() const {
		return blue_material_;
	}

	inline const gef::Material& orange_material() const {
		return orange_material_;
	}

	inline const gef::Material& grey_material() const {
		return grey_material_;
	}

	

protected:
	gef::Platform& platform_;

	gef::Mesh* default_cube_mesh_;
	gef::Mesh* default_sphere_mesh_;

	gef::Material red_material_;
	gef::Material blue_material_;
	gef::Material green_material_;
	gef::Material grey_material_;
	gef::Material orange_material_;

	
};

#endif // _PRIMITIVE_BUILDER_H