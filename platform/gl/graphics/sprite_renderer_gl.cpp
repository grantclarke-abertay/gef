//
// Created by grant on 31/10/17.
//

#include "sprite_renderer_gl.h"
#include <graphics/vertex_buffer.h>
#include <system/platform.h>
#include <graphics/texture.h>
#include <graphics/shader_interface.h>
#include <graphics/sprite.h>

#include <GL/gl.h>

gef::SpriteRenderer* gef::SpriteRenderer::Create(Platform &platform)
{
    return new gef::SpriteRendererGL(platform);
}

gef::SpriteRendererGL::SpriteRendererGL(gef::Platform &platform)
    : gef::SpriteRenderer(platform)
    , vertex_buffer_(nullptr)
    , default_shader_(platform)
{
    vertex_buffer_ = gef::VertexBuffer::Create(platform);

    float vertices[] = {-0.5f,-0.5f,0.0f, 0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f,    // triangle 1
                          -0.5f,-0.5f,0.0f, 0.5f,0.5f,0.0f, -0.5f,0.5f,0.0f};   // triangle 2
    vertex_buffer_->Init(platform, vertices, 6, sizeof(float)*3);

    platform_.AddVertexBuffer(vertex_buffer_);


    default_texture_ = gef::Texture::CreateCheckerTexture(16, 1, platform);
    platform_.AddTexture(default_texture_);

    platform_.AddShader(&default_shader_);

    projection_matrix_ = platform_.OrthographicFrustum(0, platform_.width(), 0, platform_.height(), -1, 1);
}

gef::SpriteRendererGL::~SpriteRendererGL()
{
    platform_.RemoveShader(&default_shader_);

    if(vertex_buffer_)
    {
        platform_.RemoveVertexBuffer(vertex_buffer_);
        delete vertex_buffer_;
    }

    if(default_texture_)
    {
        platform_.RemoveTexture(default_texture_);
        delete default_texture_;
    }
}

void gef::SpriteRendererGL::Begin(bool clear)
{
    platform_.BeginScene();
    if(clear)
        platform_.Clear();

    vertex_buffer_->Bind(platform_);

    default_shader_.device_interface()->UseProgram();


    default_shader_.SetSceneData(projection_matrix_);

    default_shader_.device_interface()->SetVertexFormat();

    glDisable( GL_CULL_FACE );

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, platform_.width(), platform_.height());
    //checkGlError("glViewport");
}

void gef::SpriteRendererGL::DrawSprite(const gef::Sprite &sprite)
{
    const Texture* texture = sprite.texture();
    if(!texture)
        texture = default_texture_;
    default_shader_.SetSpriteData(sprite, texture);
    default_shader_.device_interface()->SetVariableData();

    default_shader_.device_interface()->BindTextureResources(platform_);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    default_shader_.device_interface()->UnbindTextureResources(platform_);
}

void gef::SpriteRendererGL::End()
{
    vertex_buffer_->Unbind(platform_);

    platform_.EndScene();
}
