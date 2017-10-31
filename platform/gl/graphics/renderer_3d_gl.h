//
// Created by grant on 31/10/17.
//

#ifndef GL_RENDERER_3D_GL_H
#define GL_RENDERER_3D_GL_H

#include <graphics/renderer_3d.h>
#include <graphics/primitive.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace gef
{
    class Renderer3DGL : public Renderer3D
    {
    public:
        Renderer3DGL(Platform &platform);

        ~Renderer3DGL() override;

        void Begin(bool clear) override;

        void End() override;

        void DrawMesh(const MeshInstance &mesh_instance) override;

        void DrawMesh(const Mesh &mesh, const gef::Matrix44 &matrix) override;

        void DrawPrimitive(const MeshInstance &mesh_instance, Int32 primitive_index, Int32 num_indices) override;

        void SetFillMode(FillMode fill_mode) override;

        void SetDepthTest(DepthTest depth_test) override;
    protected:
        static const GLenum primitive_types[NUM_PRIMITIVE_TYPES];

        Default3DShader default_shader_;
    };

}


#endif //GL_RENDERER_3D_GL_H
