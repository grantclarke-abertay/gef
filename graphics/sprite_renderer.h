#ifndef _GEF_SPRITE_RENDERER_H
#define _GEF_SPRITE_RENDERER_H

#include <maths/matrix44.h>
#include <graphics/default_sprite_shader.h>

namespace gef
{
	// forward declarations
	class Matrix44;
	class Sprite;
	class Platform;
	class Shader;

	class SpriteRenderer
	{
	public:
		virtual ~SpriteRenderer();
		void SetShader( Shader* shader);

		virtual void Begin(bool clear = true) = 0;
		virtual void DrawSprite(const Sprite& sprite) = 0;
		virtual void End() = 0;

		inline const Matrix44& projection_matrix() const { return projection_matrix_; }
		inline void set_projection_matrix(const  Matrix44& matrix) {projection_matrix_ = matrix;}

		static SpriteRenderer* Create(Platform& platform);
	protected:
		SpriteRenderer(Platform& platform);
		void BuildSpriteShaderData(const Sprite& sprite, Matrix44& sprite_data);

		inline void set_shader( Shader* shader) { shader_ = shader; }

		Platform& platform_;
		Matrix44 projection_matrix_;

		Shader* shader_;
		DefaultSpriteShader default_shader_;
	};
}
#endif // _GEF_SPRITE_RENDERER_H
