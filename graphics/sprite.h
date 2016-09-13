#ifndef _GEF_SPRITE_H
#define _GEF_SPRITE_H

#include <gef.h>
#include <maths/vector2.h>
#include <maths/vector4.h>

namespace gef
{

class Sprite
{
public:
	/// Sprite class constructor
	///
	Sprite();

	/// @param[in] the position of the sprite
	inline void set_position(const Vector4& position) { position_ = position; }
	inline void set_position(const float x, const float y, const float z) { position_ = Vector4(x, y, z); }
	/// @return the position of the sprite
	inline const Vector4& position() const {return position_; }
	/// @param[in] the width of the sprite
	inline void set_width(const float width) { width_ = width; }
	/// @return the width of the sprite
	inline const float width() const {return width_; }
	/// @param[in] the height of the sprite
	inline void set_height(const float height) { height_ = height; }
	/// @return the height of the sprite
	inline const float height() const {return height_; }
	/// @param[in] the colour of the sprite (ABGR)
	inline void set_colour(const UInt32 colour) { colour_ = colour; }
	/// @return the colour of the sprite (ABGR)
	inline const UInt32 colour() const {return colour_; }
	/// @param[in] the rotation of the sprite, in radians
	inline void set_rotation(const float rotation) { rotation_ = rotation; }
	/// @return the rotation of the sprite, in radians
	inline const float rotation() const {return rotation_; }

	inline void set_texture(const class Texture* const texture) { texture_ = texture; }
	inline const class Texture* const texture() const { return texture_; }
	inline void set_uv_position(const Vector2& uv_position) { uv_position_ = uv_position; }
	inline const Vector2& uv_position() const {return uv_position_; }
	inline void set_uv_width(const float uv_width) { uv_width_ = uv_width; }
	inline const float uv_width() const {return uv_width_; }
	inline void set_uv_height(const float uv_height) { uv_height_ = uv_height; }
	inline const float uv_height() const {return uv_height_; }
protected:
	Vector4 position_;				// position on screen (z value used for depth sorting sprites)
	float width_;					// width of the sprite
	float height_;					// height of the sprite
	UInt32 colour_;					// colour of the sprite
	float rotation_;				// rotation angle of the sprite (in radians)

	gef::Vector2 uv_position_;
	float uv_width_;
	float uv_height_;
	const class Texture* texture_;
};

}
#endif // _GEF_SPRITE_H
