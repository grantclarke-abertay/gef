//
// Created by grant on 30/10/17.
//

#include "platform_gl.h"

std::string gef::PlatformGL::FormatFilename(const char *filename) const
{
    return std::string(filename);
}

gef::Matrix44
gef::PlatformGL::PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance,
                                          const float far_distance) const
{
    gef::Matrix44 projection_matrix;
    projection_matrix.PerspectiveFovGL(fov, aspect_ratio, near_distance, far_distance);
    return projection_matrix;
}

gef::Matrix44
gef::PlatformGL::PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom,
                                              const float near_distance, const float far_distance) const
{
    gef::Matrix44 projection_matrix;
    projection_matrix.PerspectiveFrustumGL(left, right, top, bottom, near_distance, far_distance);
    return projection_matrix;
}

gef::Matrix44
gef::PlatformGL::OrthographicFrustum(const float left, const float right, const float top, const float bottom,
                                     const float near_distance, const float far_distance) const
{
    gef::Matrix44 projection_matrix;
    projection_matrix.OrthographicFrustumGL(left, right, top, bottom, near_distance, far_distance);
    return projection_matrix;
}


const char *gef::PlatformGL::GetShaderDirectory() const
{
    return "gl";
}

const char *gef::PlatformGL::GetShaderFileExtension() const
{
    return "glsl";
}

