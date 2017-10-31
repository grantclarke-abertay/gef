//
// Created by grant on 31/10/17.
//

#include "texture_gl.h"
#include <cstdlib>
#include <cstring>
#include <graphics/image_data.h>

gef::Texture* gef::Texture::Create(const Platform &platform, const ImageData &image_data)
{
    return new TextureGL(platform, image_data);
}

gef::TextureGL::TextureGL() :
        texture_id_(0),
        image_data_buffer_(NULL)
{
}

gef::TextureGL::TextureGL(const gef::Platform &platform, const gef::ImageData &image_data) : Texture(platform,
                                                                                                     image_data)
{
    // get the size of the texture data
    const UInt32 data_size = image_data.width()*image_data.height()*4;
    image_data_buffer_ = new UInt8[data_size];

    if(image_data.image())
        memcpy(image_data_buffer_, image_data.image(),data_size);

    width_ = image_data.width();
    height_ = image_data.height();

    InitResources();
}

gef::TextureGL::~TextureGL()
{
    ReleaseResources();
    DeleteArrayNull(image_data_buffer_);
}

void gef::TextureGL::Bind(const gef::Platform &platform, const int texture_stage_num) const
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void gef::TextureGL::Unbind(const gef::Platform &platform, const int texture_stage_num) const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void gef::TextureGL::InitResources()
{
    CreateTexture(width_, height_, (UInt8*)image_data_buffer_, true);
}

void gef::TextureGL::ReleaseResources()
{
    glDeleteTextures(1, &texture_id_);
    texture_id_ = 0;
}

bool gef::TextureGL::CreateTexture(Int32 width, Int32 height, UInt8 *image_data, bool has_alpha)
{
    glGenTextures(1, &texture_id_);
    glBindTexture(GL_TEXTURE_2D, texture_id_);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}




