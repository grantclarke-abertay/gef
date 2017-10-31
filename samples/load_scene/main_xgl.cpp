#include <iostream>
#include "scene_app.h"
#include <platform/xgl/system/platform_xgl.h>

int main()
{
    gef::PlatformXGL platform_xgl;
    SceneApp myApplication(platform_xgl);

    myApplication.Run();
    return 0;
}
