//
// Created by grant on 30/10/17.
//

#include "vertex_buffer_gl.h"
#include <cstdlib>
#include <cstring>


gef::VertexBuffer* gef::VertexBuffer::Create(Platform& platform)
{
    return new gef::VertexBufferGL();
}

gef::VertexBufferGL::VertexBufferGL() :
        vertex_buffer_object_(0)
{

}

gef::VertexBufferGL::~VertexBufferGL()
{
    ReleaseResources();
    free(vertex_data_);
}

bool gef::VertexBufferGL::Init(const gef::Platform &platform, const void *vertices, const UInt32 num_vertices,
                               const UInt32 vertex_byte_size, const bool read_only)
{
    num_vertices_ = num_vertices;
    vertex_byte_size_ = vertex_byte_size;

    if(read_only)
        usage_ = GL_STATIC_DRAW;
    else
    {
        // GL_DYNAMIC_DRAW or GL_STREAM_DRAW
        usage_ = GL_STREAM_DRAW;
    }

    // need to keep a copy of the vertex data in main memory for android
    // which application loses focuses, the vertex buffer object will need to be recreated
    Int32 num_bytes = num_vertices_*vertex_byte_size_;
    vertex_data_ = malloc(num_bytes);

    if(vertices)
        memcpy(vertex_data_, vertices, num_bytes);

    InitResources();

    return true;
}

bool gef::VertexBufferGL::Update(const gef::Platform &platform)
{
    if(vertex_data_)
    {
        Bind(platform);
        glBufferSubData(GL_ARRAY_BUFFER, 0, num_vertices_*vertex_byte_size_, vertex_data_);
        Unbind(platform);
    }

    return false;
}

void gef::VertexBufferGL::Bind(const gef::Platform &platform) const
{
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
}

void gef::VertexBufferGL::Unbind(const gef::Platform &platform) const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gef::VertexBufferGL::InitResources()
{
    glGenBuffers(1, &vertex_buffer_object_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object_);
    glBufferData(GL_ARRAY_BUFFER, num_vertices_*vertex_byte_size_, vertex_data_, usage_);

    // unbind buffer
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void gef::VertexBufferGL::ReleaseResources()
{
    if (vertex_buffer_object_)
    {
        glDeleteBuffers(1, &vertex_buffer_object_ );
        vertex_buffer_object_ = 0;
    }
}
