//
// Created by grant on 31/10/17.
//

#include "renderer_3d_gl.h"
#include <graphics/texture.h>
#include <system/platform.h>
#include <graphics/mesh_instance.h>
#include <graphics/mesh.h>
#include <graphics/shader_interface.h>
#include <graphics/vertex_buffer.h>
#include <graphics/index_buffer.h>
#include "index_buffer_gl.h"


gef::Renderer3D* gef::Renderer3D::Create(Platform &platform)
{
    return new gef::Renderer3DGL(platform);
}

const GLenum gef::Renderer3DGL::primitive_types[NUM_PRIMITIVE_TYPES] =
        {
                GL_TRIANGLES,
                GL_TRIANGLE_STRIP,
                GL_LINES,

        };

gef::Renderer3DGL::Renderer3DGL(gef::Platform &platform)
    : gef::Renderer3D(platform)
    ,default_shader_(platform)
{
    platform_.AddShader(&default_shader_);
    shader_ = &default_shader_;

    projection_matrix_.SetIdentity();
}

gef::Renderer3DGL::~Renderer3DGL()
{
    platform_.RemoveShader(&default_shader_);
}

void gef::Renderer3DGL::Begin(bool clear)
{
    platform_.BeginScene();
    if(clear)
        platform_.Clear();

    glDisable( GL_CULL_FACE );
    glCullFace(GL_BACK);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, platform_.width(), platform_.height());
}

void gef::Renderer3DGL::End()
{
    platform_.EndScene();
}

void gef::Renderer3DGL::DrawMesh(const gef::MeshInstance &mesh_instance)
{
    // set up the shader data for default shader
    if (shader_ == &default_shader_)
        default_shader_.SetSceneData(default_shader_data_, view_matrix_, projection_matrix_);

    const Mesh* mesh = mesh_instance.mesh();
    if(mesh != NULL)
    {
        set_world_matrix(mesh_instance.transform());

        const VertexBuffer* vertex_buffer = mesh->vertex_buffer();
        //ShaderGL* shader_GL = static_cast<ShaderGL*>(shader_);

        if(vertex_buffer && shader_)
        {
            shader_->SetMeshData(mesh_instance);

            shader_->device_interface()->UseProgram();
            vertex_buffer->Bind(platform_);

            // vertex format must be set after the vertex buffer is bound
            shader_->device_interface()->SetVertexFormat();

            for(UInt32 primitive_index=0;primitive_index<mesh->num_primitives();++primitive_index)
            {
                const Primitive* primitive = mesh->GetPrimitive(primitive_index);
                const IndexBuffer* index_buffer = primitive->index_buffer();
                if(primitive->type() != UNDEFINED && index_buffer)
                {
                    // default texture
                    const Material* material;
                    if (override_material_)
                        material = override_material_;
                    else
                        material = primitive->material();


                    //only set default shader data if current shader is the default shader
                    shader_->SetMaterialData(material);

                    // GRC FIXME - probably want to split variable data into scene, object, primitive[material?] based
                    // rather than set all variables per primitive
                    shader_->device_interface()->SetVariableData();
                    shader_->device_interface()->BindTextureResources(platform());

                    GLenum primitive_type = primitive_types[primitive->type()];
                    index_buffer->Bind(platform_);
                    const IndexBufferGL* index_buffer_gl = static_cast<const IndexBufferGL*>(primitive->index_buffer());
                    glDrawElements(primitive_type, index_buffer->num_indices(), index_buffer_gl->type(), 0);
                    index_buffer->Unbind(platform_);

                    shader_->device_interface()->UnbindTextureResources(platform());
                }
            }

            vertex_buffer->Unbind(platform_);
            shader_->device_interface()->ClearVertexFormat();
        }
    }

}

void gef::Renderer3DGL::DrawMesh(const gef::Mesh &mesh, const gef::Matrix44 &matrix)
{
    // set up the shader data for default shader
    if (shader_ == &default_shader_)
        default_shader_.SetSceneData(default_shader_data_, view_matrix_, projection_matrix_);

    set_world_matrix(matrix);

    const VertexBuffer* vertex_buffer = mesh.vertex_buffer();

    if(vertex_buffer && shader_)
    {
        shader_->SetMeshData(matrix);

        shader_->device_interface()->UseProgram();
        vertex_buffer->Bind(platform_);

        // vertex format must be set after the vertex buffer is bound
        shader_->device_interface()->SetVertexFormat();

        for(UInt32 primitive_index=0;primitive_index<mesh.num_primitives();++primitive_index)
        {
            const Primitive* primitive = mesh.GetPrimitive(primitive_index);
            const IndexBuffer* index_buffer = primitive->index_buffer();
            if(primitive->type() != UNDEFINED && index_buffer)
            {
                // default texture
                const Material* material;
                if (override_material_)
                    material = override_material_;
                else
                    material = primitive->material();


                //only set default shader data if current shader is the default shader
                shader_->SetMaterialData(material);

                // GRC FIXME - probably want to split variable data into scene, object, primitive[material?] based
                // rather than set all variables per primitive
                shader_->device_interface()->SetVariableData();
                shader_->device_interface()->BindTextureResources(platform());

                GLenum primitive_type = primitive_types[primitive->type()];
                index_buffer->Bind(platform_);
                const IndexBufferGL* index_buffer_gl = static_cast<const IndexBufferGL*>(primitive->index_buffer());
                glDrawElements(primitive_type, index_buffer->num_indices(), index_buffer_gl->type(), 0);
                index_buffer->Unbind(platform_);

                shader_->device_interface()->UnbindTextureResources(platform());
            }
        }

        vertex_buffer->Unbind(platform_);
        shader_->device_interface()->ClearVertexFormat();
    }

}

void gef::Renderer3DGL::DrawPrimitive(const gef::MeshInstance &mesh_instance, Int32 primitive_index, Int32 num_indices)
{

}

void gef::Renderer3DGL::SetFillMode(gef::Renderer3D::FillMode fill_mode)
{

}

void gef::Renderer3DGL::SetDepthTest(gef::Renderer3D::DepthTest depth_test)
{

}
