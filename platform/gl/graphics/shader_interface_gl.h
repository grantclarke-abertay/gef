//
// Created by grant on 31/10/17.
//

#ifndef GL_SHADER_INTERFACE_GL_H
#define GL_SHADER_INTERFACE_GL_H

#include <graphics/shader_interface.h>
#include <GL/glew.h>
#include <GL/gl.h>

namespace gef
{
    class ShaderInterfaceGL : public ShaderInterface
    {
    public:
        ShaderInterfaceGL();

        ~ShaderInterfaceGL() override;

        bool CreateProgram() override;

        void CreateVertexFormat() override;

        void UseProgram() override;

        void SetVariableData() override;

        void SetVertexFormat() override;

        void ClearVertexFormat() override;

        void BindTextureResources(const Platform &platform) const override;

        void UnbindTextureResources(const Platform &platform) const override;

        void InitResources();

        void ReleaseResources();

    protected:
        struct AttributeData
        {
            GLuint location;
            GLint size;
            GLenum type;
            const GLvoid* offset;
        };

        GLuint CreateShader(GLenum shaderType, const char* source, UInt32 source_length);
        void SetShaderVariableData(const std::vector<ShaderVariable>& variables, const UInt8* variable_data, Int32 variable_index);

        static 	Int32 GetAttributeTypeSize(VariableType type);
        static 	GLenum GetAttributeType(VariableType type);

        GLuint program_;
        std::vector<AttributeData> attributes_data_;
        std::vector<GLuint> uniform_locations_;
        std::vector<GLuint> texture_sampler_uniform_locations_;
    };
}


#endif //GL_SHADER_INTERFACE_GL_H
