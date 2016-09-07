#include "material.h"
#include <stdlib.h>

namespace gef
{
	Material::Material() : 
		texture_(NULL),
		colour_(0xffffffff)
	{
	}

	Material::~Material()
	{
	}
}