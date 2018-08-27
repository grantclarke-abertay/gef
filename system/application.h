#ifndef _GEF_APPLICATION_H
#define _GEF_APPLICATION_H

namespace gef
{
	// forward declarations
	class Platform;

	class Application
	{
	public:
		Application(Platform& platform);
		virtual ~Application();
		void Run();
		virtual void Init() = 0;
		virtual void CleanUp() = 0;
		virtual bool Update(float ticks) = 0;
		virtual void Render() = 0;
		virtual void PostFrameEnd() {};

		inline Platform& platform() { return platform_; }

	protected:
		class Platform& platform_;
	private:
		bool running_;
	};
}

#endif // _GEF_APPLICATION_H
