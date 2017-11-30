#include <iostream>
#include "assimp_viewer.h"
#include <platform/glfw/system/platform_glfw.h>

#include "imgui_impl_glfw_gl3.h"

void ImGui_NewFrame()
{
    ImGui_ImplGlfwGL3_NewFrame();
}


int main()
{
    gef::PlatformGLFW platform_xgl;

    ImGui_ImplGlfwGL3_Init(platform_xgl.window(), true);


    AssimpViewer myApplication(platform_xgl);

    myApplication.Run();

    ImGui_ImplGlfwGL3_Shutdown();
    return 0;
}