//
// Created by grant on 30/10/17.
//

#ifndef GL_PLATFORM_GL_H
#define GL_PLATFORM_GL_H

#include <system/platform.h>

namespace gef
{
    class PlatformGL : public Platform
    {
    public:
        std::string FormatFilename(const char *filename) const override;

        Matrix44 PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance,
                                          const float far_distance) const override;

        Matrix44 PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom,
                                              const float near_distance, const float far_distance) const override;

        Matrix44 OrthographicFrustum(const float left, const float right, const float top, const float bottom,
                                     const float near_distance, const float far_distance) const override;

        const char *GetShaderDirectory() const override;

        const char *GetShaderFileExtension() const override;
    };
}


#endif //GL_PLATFORM_GL_H
