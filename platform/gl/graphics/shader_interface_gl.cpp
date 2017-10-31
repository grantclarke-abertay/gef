//
// Created by grant on 31/10/17.
//

#include "shader_interface_gl.h"
#include <graphics/texture.h>
#include <system/platform.h>
#include <system/debug_log.h>
#include <cstring>

gef::ShaderInterface* gef::ShaderInterface::Create(const Platform &platform)
{
    return new gef::ShaderInterfaceGL();
}

gef::ShaderInterfaceGL::ShaderInterfaceGL()
    : program_(0)
{

}

gef::ShaderInterfaceGL::~ShaderInterfaceGL()
{
    ReleaseResources();
}

bool gef::ShaderInterfaceGL::CreateProgram()
{
    bool success = true;
#if 1
    //
    // allocate memory to store local copy of variable data
    //
    AllocateVariableData();


    //
    // Create shaders
    //
    GLuint vertexShader = 0;
    GLuint pixelShader = 0;

    // if there is vertex shader source the create the vertex shader
    if((vs_shader_source_ != NULL) && (vs_shader_source_size_ > 0))
    {
        vertexShader = CreateShader(GL_VERTEX_SHADER, vs_shader_source_, vs_shader_source_size_);
        if (!vertexShader)
        {
            DebugOut("ShaderInterfaceGL::CreateProgram: failed to create vertex shader from %s\n", vs_shader_source_);
            exit(-1);
        }
    }

    // if there is a pixel shader source then create the pixel shader
    if((ps_shader_source_ != NULL) && (ps_shader_source_size_ > 0))
    {
        pixelShader = CreateShader(GL_FRAGMENT_SHADER, ps_shader_source_, ps_shader_source_size_);
        if (!pixelShader)
        {
            DebugOut("ShaderInterfaceGL::CreateProgram: failed to create pixel shader from %s\n", ps_shader_source_);
            exit(-1);
        }
    }

    //
    // Create shader program
    //
    GLuint program = glCreateProgram();
    if (program)
    {
        if(vertexShader)
        {
            glAttachShader(program, vertexShader);
            //        	checkGlError("glAttachShader");
        }

        if(pixelShader)
        {
            glAttachShader(program, pixelShader);
            //     checkGlError("glAttachShader");
        }

        glLinkProgram(program);
        GLint linkStatus = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE)
        {
            GLint bufLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength)
            {
                char* buf = (char*) malloc(bufLength);
                if (buf)
                {
                    glGetProgramInfoLog(program, bufLength, NULL, buf);
                    //                    LOGE("Could not link program:\n%s\n", buf);
                    free(buf);
                }
            }
            glDeleteProgram(program);
            program = 0;
        }

        if(vertexShader)
        {
            glDetachShader(program, vertexShader);
            glDeleteShader(vertexShader);
        }

        if(pixelShader)
        {
            glDetachShader(program, pixelShader);
            glDeleteShader(pixelShader);
        }
    }
    program_ = program;

    //
    // Find shader parameters and variables
    //

    // go through all vertex shader parameters and find them in the shader program
    Int32 offset = 0;
    for(const auto & shader_parameter : parameters_)
    {
        AttributeData attribute_data;
        attribute_data.location = glGetAttribLocation(program_, shader_parameter.name.c_str());
        if(attribute_data.location == 0xffffffff)
            DebugOut("ShaderInterfaceGL::CreateProgram: %s attribute not found\n", shader_parameter.name.c_str());
        attribute_data.size = GetAttributeTypeSize(shader_parameter.type);
        attribute_data.type = GetAttributeType(shader_parameter.type);
        attribute_data.offset = (const GLvoid*)shader_parameter.byte_offset;
        offset += GetTypeSize(shader_parameter.type);
        attributes_data_.push_back(attribute_data);
    }

    // go through all shader variables and find them in the shader program
    for(const auto & shader_variable : vertex_shader_variables_)
    {
        GLuint uniform_location = glGetUniformLocation(program_, shader_variable.name.c_str());
        if(uniform_location == 0xffffffff)
            DebugOut("ShaderInterfaceGL::CreateProgram: %s uniform not found\n", shader_variable.name.c_str());
        uniform_locations_.push_back(uniform_location);
    }
    for(const auto & shader_variable : pixel_shader_variables_)
    {
        GLuint uniform_location = glGetUniformLocation(program_, shader_variable.name.c_str());
        if(uniform_location == 0xffffffff)
            DebugOut("ShaderInterfaceGL::CreateProgram: %s uniform not found\n", shader_variable.name.c_str());
        uniform_locations_.push_back(uniform_location);
    }

    // go through all shader variables and find them in the shader program
    for(const auto & texture_sampler : texture_samplers_)
    {
        GLuint uniform_location = glGetUniformLocation(program_, texture_sampler.name.c_str());
        if(uniform_location == 0xffffffff)
            DebugOut("ShaderInterfaceGL::CreateProgram: %s texture sampler uniform not found\n", texture_sampler.name.c_str());
        texture_sampler_uniform_locations_.push_back(uniform_location);
    }
#endif
    return success;}

void gef::ShaderInterfaceGL::CreateVertexFormat()
{
    // don't have to do any there for OpenGL
}

void gef::ShaderInterfaceGL::UseProgram()
{
    glUseProgram(program_);
}

void gef::ShaderInterfaceGL::SetVariableData()
{
    Int32 variable_index = 0;
    SetShaderVariableData(vertex_shader_variables_, vertex_shader_variable_data_, variable_index);
    variable_index += vertex_shader_variables_.size();
    SetShaderVariableData(pixel_shader_variables_, pixel_shader_variable_data_, variable_index);
    variable_index += pixel_shader_variables_.size();

    // TODO - this needs checked when more than one texture stage is being used
    Int32 texture_sampler_index = 0;
    for(const auto & texture_sampler_uniform_location : texture_sampler_uniform_locations_)
    {
        glUniform1i(texture_sampler_uniform_location, texture_sampler_index++);
    }
}

void gef::ShaderInterfaceGL::SetVertexFormat()
{
    for(UInt32 vertex_attribute=0;vertex_attribute<GL_MAX_VERTEX_ATTRIBS;++vertex_attribute)
    {
        if(vertex_attribute < parameters_.size())
        {
            glEnableVertexAttribArray(vertex_attribute);

            const AttributeData& attribute_data = attributes_data_[vertex_attribute];
            glVertexAttribPointer(attribute_data.location,attribute_data.size,attribute_data.type,GL_FALSE,vertex_size_, attribute_data.offset);
        }
        else
            glDisableVertexAttribArray(vertex_attribute);
    }
}

void gef::ShaderInterfaceGL::ClearVertexFormat()
{
    for(UInt32 vertex_attribute=0;vertex_attribute<GL_MAX_VERTEX_ATTRIBS;++vertex_attribute)
        if(vertex_attribute < parameters_.size())
            glDisableVertexAttribArray(vertex_attribute);
}

void gef::ShaderInterfaceGL::BindTextureResources(const gef::Platform &platform) const
{
    Int32 texture_stage_num = 0;
    for(const auto & texture_sampler : texture_samplers_)
    {
        if (texture_sampler.texture)
            texture_sampler.texture->Bind(platform, texture_stage_num);
        else if (platform.default_texture())
            platform.default_texture()->Bind(platform, texture_stage_num);
        texture_stage_num++;
    }
}

void gef::ShaderInterfaceGL::UnbindTextureResources(const gef::Platform &platform) const
{
    Int32 texture_stage_num = 0;
    for(auto texture_sampler : texture_samplers_)
    {
        if(texture_sampler.texture)
            texture_sampler.texture->Unbind(platform, texture_stage_num);
        texture_stage_num++;
    }
}


void gef::ShaderInterfaceGL::InitResources()
{
    CreateProgram();
}

void gef::ShaderInterfaceGL::ReleaseResources()
{
    if(program_ != 0)
    {
        glDeleteProgram(program_);
        program_ = 0;
    }
}

GLuint gef::ShaderInterfaceGL::CreateShader(GLenum shaderType, const char *source, UInt32 source_length)
{
    GLuint shader = 0;

    shader = glCreateShader(shaderType);
    if (shader) {

        const GLchar* sources[3] = {
                // Define GLSL version
#ifdef GL_ES_VERSION_2_0
                "#version 100\n"
#else
                "#version 120\n"
#endif
                ,
                // GLES2 precision specifiers
#ifdef GL_ES_VERSION_2_0
        // Define default float precision for fragment shaders:
    (shaderType == GL_FRAGMENT_SHADER) ?
    "#ifdef GL_FRAGMENT_PRECISION_HIGH\n"
    "precision highp float;           \n"
    "#else                            \n"
    "precision mediump float;         \n"
    "#endif                           \n"
    : ""
    // Note: OpenGL ES automatically defines this:
    // #define GL_ES
#else
                // Ignore GLES 2 precision specifiers:
                "#define lowp   \n"
                        "#define mediump\n"
                        "#define highp  \n"
#endif
                ,
                source};

        GLint sources_length[3];

        sources_length[0] = strlen(sources[0]);
        sources_length[1] = strlen(sources[1]);
        sources_length[2] = source_length;

        glShaderSource(shader, 3, sources, sources_length);
        glCompileShader(shader);
        GLint compiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
        if (!compiled) {
            GLint info_length = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_length);
            if (info_length)
            {
                char* buf = (char*) malloc(info_length);
                if (buf)
                {
                    glGetShaderInfoLog(shader, info_length, NULL, buf);
                    DebugOut("Could not compile shader %d:\n%s\n",
                             shaderType, buf);
                    free(buf);
                }
                glDeleteShader(shader);
                shader = 0;
            }
        }
    }

    return shader;
}

Int32 gef::ShaderInterfaceGL::GetAttributeTypeSize(gef::ShaderInterface::VariableType type)
{
    Int32 size=0;

    switch(type)
    {
        case kFloat:
            size = 1;
            break;
        case kVector2:
            size = 2;
            break;
        case kVector3:
            size = 3;
            break;
        case kVector4:
            size = 4;
            break;
        case kMatrix44:
            size = 16;
            break;
    }

    return size;
}

GLenum gef::ShaderInterfaceGL::GetAttributeType(gef::ShaderInterface::VariableType type)
{
    GLenum attribute_type;
    switch(type)
    {
        case kFloat:
        case kVector2:
        case kVector3:
        case kVector4:
        case kMatrix44:
            attribute_type = GL_FLOAT;
            break;
    }

    return attribute_type;
}

void gef::ShaderInterfaceGL::SetShaderVariableData(const std::vector<gef::ShaderInterface::ShaderVariable> &variables,
                                                   const UInt8 *variable_data, Int32 variable_index)
{
    for(const auto & shader_variable : variables)
    {
        const UInt8* data = &variable_data[shader_variable.byte_offset];

        GLuint uniform_location = uniform_locations_[variable_index];

        switch(shader_variable.type)
        {
            case kFloat:
                glUniform1fv(uniform_location, shader_variable.count, reinterpret_cast<const float *>(data));
                break;
            case kVector2:
                glUniform2fv(uniform_location, shader_variable.count, reinterpret_cast<const float *>(data));
                break;
            case kVector3:
                glUniform3fv(uniform_location, shader_variable.count, reinterpret_cast<const float *>(data));
                break;
            case kVector4:
                glUniform4fv(uniform_location, shader_variable.count, reinterpret_cast<const float *>(data));
                break;
            case kMatrix44:
                glUniformMatrix4fv(uniform_location, shader_variable.count, GL_FALSE, reinterpret_cast<const float *>(data));
                break;
        }

        ++variable_index;
    }
}
