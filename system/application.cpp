#include <system/application.h>
#include <system/platform.h>

namespace gef
{

Application::Application(Platform& platform) :
	platform_(platform),
	running_(true)
{
}

Application::~Application()
{
}

void Application::Run()
{
	// INITIALISATION
	Init();

	const float kTargetFrameTime = 1.0f / 60.0f;
	const float kTooLargeFrameTime = 1.0f / 10.f;

	// start off assuming target frame rate
	
	// as this is the first frame there is no previous
	// frame time, but GetFrameTime() needs to 
	// be called to get time at the start of the first frame
	// we'll ignore the value returned by GetFrameTime
	// and use the target frame rate instead
	float previous_frame_time = platform_.GetFrameTime();
	previous_frame_time = kTargetFrameTime;

	while(running_)
	{
		running_ = platform_.Update();
		if(running_)
		{
			// check to see if frame time is too large
			// if it is we need use the target frame rate 
			// as using a large frame time compound any frame rate drop
			if(previous_frame_time > kTooLargeFrameTime)
				previous_frame_time = kTargetFrameTime;

			// UPDATE
			running_ = Update(previous_frame_time);

			// RENDER
			if(platform_.ReadyToRender())
			{
				platform_.PreRender();
				Render();
				platform_.PostRender();
			}

			previous_frame_time = platform_.GetFrameTime();
		}
	}

	// CLEAN UP
	CleanUp();
    //platform_.Release();
}

}
