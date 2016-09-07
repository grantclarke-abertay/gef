#include <graphics/sprite.h>
#include <cstddef>

namespace gef
{

Sprite::Sprite() :
	colour_(0xffffffff),
	rotation_(0.0f),
	uv_position_(0.0f, 0.0f),
	uv_width_(1.0f),
	uv_height_(1.0f),
	texture_(NULL)
{
}

}