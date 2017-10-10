#include <graphics/sprite_renderer.h>
#include <graphics/sprite.h>
#include <graphics/colour.h>
#include <maths/vector2.h>
#include <maths/matrix44.h>
//#include <libdbg.h>
#include <cstdlib>
#include <math.h>
#include <graphics/shader.h>

namespace gef
{

SpriteRenderer::SpriteRenderer(Platform& platform) :
platform_(platform),
	shader_(NULL),
	default_shader_(platform_)
{
	//SCE_DBG_ASSERT(platform_ != NULL);
}


SpriteRenderer::~SpriteRenderer()
{
}

void SpriteRenderer::SetShader( Shader* shader)
{
	if(shader == NULL)
		set_shader(&default_shader_);
	else
		set_shader(shader);
}

void SpriteRenderer::BuildSpriteShaderData(const Sprite& sprite, Matrix44& sprite_data)
{
		Vector2 sprite_origin(0.5f, 0.5f);
		Vector2 sprite_uv_origin(0.0f, 0.0f);
		Vector2 sprite_uv_size(1.0f, 1.0f);

		sprite_data.set_m(2,0,sprite.position().x());
        sprite_data.set_m(2,1,sprite.position().y());

		// origin ( not going to pass origin in so we can use for something else
//        sprite_data.set_m(2,2, sprite_origin.x);
//        sprite_data.set_m(2,3, sprite_origin.y);

		// depth
		sprite_data.set_m(2,2,sprite.position().z());

		// scale*rotation
		if(sprite.rotation() == 0)
		{
			sprite_data.set_m(0,0,sprite.width());
			sprite_data.set_m(0,1,0.0f);
			sprite_data.set_m(1,0,0.0f);
			sprite_data.set_m(1,1,sprite.height());
		}
		else
		{
			sprite_data.set_m(0,0,cosf(sprite.rotation())*sprite.width());
			sprite_data.set_m(0,1,sinf(sprite.rotation())*sprite.width());
			sprite_data.set_m(1,0,-sinf(sprite.rotation())*sprite.height());
			sprite_data.set_m(1,1,cosf(sprite.rotation())*sprite.height());
		}

        // Source rectangle
		sprite_data.set_m(0,2,sprite.uv_position().x);
        sprite_data.set_m(0,3,sprite.uv_position().y);
		sprite_data.set_m(1,2,sprite.uv_width());
        sprite_data.set_m(1,3,sprite.uv_height());

		Colour colour;
		colour.SetFromAGBR(sprite.colour());

        sprite_data.set_m(3,0,colour.r);
        sprite_data.set_m(3,1,colour.g);
        sprite_data.set_m(3,2,colour.b);
        sprite_data.set_m(3,3,colour.a);
}


}