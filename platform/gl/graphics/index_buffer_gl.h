//
// Created by grant on 31/10/17.
//

#ifndef GL_INDEX_BUFFER_GL_H
#define GL_INDEX_BUFFER_GL_H

#include <graphics/index_buffer.h>
#include <GL/glew.h>
#include <GL/gl.h>


namespace gef
{
    class IndexBufferGL : public gef::IndexBuffer
    {
    public:
        IndexBufferGL();

        ~IndexBufferGL() override;

        bool Init(const Platform &platform, const void *indices, const UInt32 num_indices, const UInt32 index_byte_size,
                  const bool read_only) override;

        void Bind(const Platform &platform) const override;

        void Unbind(const Platform &platform) const override;

        bool Update(const Platform &platform) override;

        void InitResources();
        void ReleaseResources();
    protected:
        GLuint index_buffer_object_;
        GLenum usage_;
        GLenum type_;
    public:
        inline GLenum type() const
        {
            return type_;
        }
    };
}


#endif //GL_INDEX_BUFFER_GL_H
