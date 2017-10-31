//
// Created by grant on 31/10/17.
//

#include "platform_xgl.h"
#include <graphics/texture.h>

#include <pthread.h>

// dummy call to pthread function to make sure pthread library gets linked in
// OpenGL library needs pthreads library
void pthread_dummy()
{
    int j;
    j = pthread_getconcurrency();
}

GLint                   att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

gef::PlatformXGL::PlatformXGL()
    : dpy(NULL)
    , vi(NULL)
    , clock_last_frame_(0)
{
    dpy = XOpenDisplay(NULL);

    if(dpy == NULL) {
        printf("\n\tcannot connect to X server\n\n");
        exit(0);
    }


    root = DefaultRootWindow(dpy);

    vi = glXChooseVisual(dpy, 0, att);

    if(vi == NULL) {
        printf("\n\tno appropriate visual found\n\n");
        exit(0);
    }
    else {
        printf("\n\tvisual %p selected\n", (void *)vi->visualid); /* %p creates hexadecimal output like in glxinfo */
    }

    cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);

    swa.colormap = cmap;
    swa.event_mask = ExposureMask | KeyPressMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask;

    win = XCreateWindow(dpy, root, 0, 0, 960, 544, 0, vi->depth, InputOutput, vi->visual, CWColormap | CWEventMask, &swa);

    XMapWindow(dpy, win);
    XStoreName(dpy, win, "VERY SIMPLE APPLICATION");

    XWindowAttributes       gwa;
    XGetWindowAttributes(dpy, win, &gwa);
    set_width(gwa.width);
    set_height(gwa.height);

    glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
    glXMakeCurrent(dpy, win, glc);

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

    // make a call to the pthreads dummy function to ensure pthreads library gets linked in
    pthread_dummy();

}

gef::PlatformXGL::~PlatformXGL()
{
    Release();
}

bool gef::PlatformXGL::Update()
{
    bool running = true;


    Atom wmDeleteMessage = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(dpy, win, &wmDeleteMessage, 1);


    if(XPending(dpy))
    {
        XEvent xev;
        XNextEvent(dpy, &xev);

        // let the touch manager handle events it is interested in

//        if(touch_input_manager_)
//            ((TouchInputManagerLinux*)touch_input_manager_)->HandleEvent(xev);

        switch (xev.type)
        {
            //case Expose:
            //    printf("Expose\n");
            //    break;

            case ClientMessage:
                if ((Atom)xev.xclient.data.l[0] == wmDeleteMessage)
                    running = false;
                break;

            default:
                break;
        }
    }

    return running;
}

float gef::PlatformXGL::GetFrameTime()
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

void gef::PlatformXGL::PreRender()
{

}

void gef::PlatformXGL::PostRender()
{
    glXSwapBuffers(dpy, win);
}

void gef::PlatformXGL::Clear() const
{
    glClearColor(render_target_clear_colour_.r, render_target_clear_colour_.g, render_target_clear_colour_.b, render_target_clear_colour_.a);
    glClearDepthf(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void gef::PlatformXGL::BeginScene() const
{

}

void gef::PlatformXGL::EndScene() const
{

}


void gef::PlatformXGL::Release()
{
//    ReleaseTouchInputManager();

    if(dpy && vi)
    {
        glXMakeCurrent(dpy, None, NULL);
        glXDestroyContext(dpy, glc);

        XUnmapWindow(dpy, win);

        XDestroyWindow(dpy, win);
        XFreeColormap(dpy, cmap);
        XFree(vi);
        vi = NULL;
        XCloseDisplay(dpy);
        dpy = NULL;
    }
}

