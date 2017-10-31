//
// Created by grant on 31/10/17.
//

#ifndef GL_SPRITE_RENDERER_GL_H
#define GL_SPRITE_RENDERER_GL_H

#include <graphics/sprite_renderer.h>

namespace gef
{
    class VertexBuffer;

    class SpriteRendererGL : public SpriteRenderer
    {
    public:
        SpriteRendererGL(Platform &platform);

        ~SpriteRendererGL() override;

        void Begin(bool clear) override;

        void DrawSprite(const Sprite &sprite) override;

        void End() override;
    protected:
        Texture* default_texture_;
        VertexBuffer* vertex_buffer_;
        DefaultSpriteShader default_shader_;
    };

}


#endif //GL_SPRITE_RENDERER_GL_H
