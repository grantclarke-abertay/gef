//
// Created by grant on 23/11/17.
//

#ifndef GLFW_PLATFORM_GLFW_H
#define GLFW_PLATFORM_GLFW_H

#include <platform/gl/system/platform_gl.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace gef
{
    class PlatformGLFW : public PlatformGL
    {
    public:
        PlatformGLFW();
        ~PlatformGLFW() override ;

        bool Update() override;

        float GetFrameTime() override;

        void PreRender() override;

        void PostRender() override;

        void Clear() const override;

        void BeginScene() const override;

        void EndScene() const override;

        inline GLFWwindow *window() const
        {
            return window_;
        }

        inline void set_window(GLFWwindow *window)
        {
            PlatformGLFW::window_ = window;
        }

    protected:
        GLFWwindow* window_;

    protected:
        UInt64		clock_last_frame_;
    };
}


#endif //GLFW_PLATFORM_GLFW_H
