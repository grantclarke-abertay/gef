//
// Created by grant on 31/10/17.
//

#ifndef GL_TEXTURE_GL_H
#define GL_TEXTURE_GL_H

#include <graphics/texture.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace gef
{
    class TextureGL : public Texture
    {
    public:
        TextureGL();
        TextureGL(const Platform &platform, const ImageData &image_data);

        ~TextureGL() override;

        void Bind(const Platform &platform, const int texture_stage_num) const override;

        void Unbind(const Platform &platform, const int texture_stage_num) const override;

        void InitResources();
        void ReleaseResources();
    protected:
        bool CreateTexture(Int32 width, Int32 height, UInt8* image_data, bool has_alpha);

        GLuint texture_id_;

        // we need to keep the texture data around for when the application
        // loses focus
        UInt8* image_data_buffer_;
        Int32 width_;
        Int32 height_;
    };
}


#endif //GL_TEXTURE_GL_H
