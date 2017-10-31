//
// Created by grant on 31/10/17.
//

#include "index_buffer_gl.h"
#include <cstdlib>
#include <cstring>


gef::IndexBuffer* gef::IndexBuffer::Create(Platform& platform)
{
    return new gef::IndexBufferGL();
}

gef::IndexBufferGL::IndexBufferGL()
    : index_buffer_object_(0)
{}

gef::IndexBufferGL::~IndexBufferGL()
{
    ReleaseResources();
}

bool gef::IndexBufferGL::Init(const gef::Platform &platform, const void *indices, const UInt32 num_indices,
                              const UInt32 index_byte_size, const bool read_only)
{
    num_indices_ = num_indices;
    index_byte_size_ = index_byte_size;

    if(read_only)
        usage_ = GL_STATIC_DRAW;
    else
    {
        // GL_DYNAMIC_DRAW or GL_STREAM_DRAW
        usage_ = GL_STREAM_DRAW;
    }

    switch(index_byte_size)
    {
        case 1:
            type_ = GL_UNSIGNED_BYTE;
            break;
        case 2:
            type_ = GL_UNSIGNED_SHORT;
            break;
        case 4:
            type_ = GL_UNSIGNED_INT;
            break;

        default:
            break;
    }

    // need to keep a copy of the index data in main memory for android
    // which application loses focuses, the index buffer object will need to be recreated
    Int32 num_bytes = num_indices_*index_byte_size_;
    index_data_ = malloc(num_bytes);
    if(indices)
        memcpy(index_data_, indices, num_bytes);

    InitResources();

    return true;
}

void gef::IndexBufferGL::Bind(const gef::Platform &platform) const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
}

void gef::IndexBufferGL::Unbind(const gef::Platform &platform) const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

bool gef::IndexBufferGL::Update(const gef::Platform &platform)
{
    if(index_data_)
    {
        Bind(platform);
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, num_indices_*index_byte_size_, index_data_);
        Unbind(platform);
    }

    return true;
}

void gef::IndexBufferGL::InitResources()
{
    glGenBuffers(1, &index_buffer_object_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_object_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices_*index_byte_size_, index_data_, GL_STATIC_DRAW);

    // unbind buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void gef::IndexBufferGL::ReleaseResources()
{
    if(index_buffer_object_)
    {
        glDeleteBuffers(1, &index_buffer_object_);
        index_buffer_object_ = 0;
    }
}




