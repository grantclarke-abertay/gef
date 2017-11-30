#include <iostream>
#include "mesh_app.h"
#include <platform/glfw/system/platform_glfw.h>

int main()
{
    gef::PlatformGLFW platform_xgl;
    MeshApp myApplication(platform_xgl);

    myApplication.Run();
    return 0;
}