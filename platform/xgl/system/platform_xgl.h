//
// Created by grant on 31/10/17.
//

#ifndef XGL_PLATFORM_XGL_H
#define XGL_PLATFORM_XGL_H

#include <platform/gl/system/platform_gl.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include <GL/glew.h>
#include<GL/gl.h>
#include<GL/glx.h>


namespace gef
{
    class PlatformXGL : public PlatformGL
    {
    public:
        PlatformXGL();

        ~PlatformXGL() override;

        bool Update() override;

        float GetFrameTime() override;

        void PreRender() override;

        void PostRender() override;

        void Clear() const override;

        void BeginScene() const override;

        void EndScene() const override;


    private:
        void Release();

        Display                 *dpy;
        Window                  root;
        XVisualInfo             *vi;
        Colormap                cmap;
        XSetWindowAttributes    swa;
        Window                  win;
        GLXContext              glc;
        UInt64		clock_last_frame_;
    };

}


#endif //XGL_PLATFORM_XGL_H
