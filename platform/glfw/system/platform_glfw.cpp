//
// Created by grant on 23/11/17.
//

#include "platform_glfw.h"
#include <graphics/texture.h>

static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error %d: %s\n", error, description);
}

gef::PlatformGLFW::PlatformGLFW()
    : window_(nullptr)
    , clock_last_frame_(0)
{
    // Setup window
    glfwSetErrorCallback(error_callback);
    if (glfwInit())
    {
        window_ = glfwCreateWindow(1280, 720, "GEF Application", NULL, NULL);
        glfwMakeContextCurrent(window_);

        set_width(1280);
        set_height(720);

        GLenum err = glewInit();
        if (GLEW_OK != err)
        {
            /* Problem: glewInit failed, something is seriously wrong. */
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
            exit(0);
        }

        // create default texture
        default_texture_ = Texture::CreateCheckerTexture(16, 1, *this);
        AddTexture(default_texture_);

    }
}

gef::PlatformGLFW::~PlatformGLFW()
{
    glfwTerminate();
}

bool gef::PlatformGLFW::Update()
{

    bool running = !glfwWindowShouldClose(window_);
    if(running)
    {
        glfwPollEvents();
    }
    return running;
}

float gef::PlatformGLFW::GetFrameTime()
{
    struct timespec gettime_now;
    if(clock_last_frame_ == 0)
    {
        clock_gettime(CLOCK_REALTIME, &gettime_now);
        clock_last_frame_ = gettime_now.tv_nsec; // in nanoseconds
    }

    clock_gettime(CLOCK_REALTIME, &gettime_now);
    UInt64 clock = gettime_now.tv_nsec; // in nanoseconds
    UInt64 delta_time = clock - clock_last_frame_;
    clock_last_frame_ = clock;

    return (float)delta_time / (float)1000000000;
}

void gef::PlatformGLFW::PreRender()
{
    int display_w, display_h;
    glfwGetFramebufferSize(window_, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
}

void gef::PlatformGLFW::PostRender()
{
    glfwSwapBuffers(window_);
}

void gef::PlatformGLFW::Clear() const
{
    glClearColor(render_target_clear_colour_.r, render_target_clear_colour_.g, render_target_clear_colour_.b, render_target_clear_colour_.a);
    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gef::PlatformGLFW::BeginScene() const
{

}

void gef::PlatformGLFW::EndScene() const
{

}


