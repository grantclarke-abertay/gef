//
// Created by grant on 30/10/17.
//

#ifndef GL_VERTEX_BUFFER_GL_H
#define GL_VERTEX_BUFFER_GL_H

#include <graphics/vertex_buffer.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace gef
{
    class VertexBufferGL : public VertexBuffer
    {
    public:
        VertexBufferGL();
        ~VertexBufferGL() override;

        bool
        Init(const Platform &platform, const void *vertices, const UInt32 num_vertices, const UInt32 vertex_byte_size,
             const bool read_only) override;

        bool Update(const Platform &platform) override;

        void Bind(const Platform &platform) const override;

        void Unbind(const Platform &platform) const override;

        void InitResources();
        void ReleaseResources();

    protected:
        GLuint vertex_buffer_object_;
        GLenum usage_;
    };
}


#endif //GL_VERTEX_BUFFER_GL_H
