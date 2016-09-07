#ifndef _RENDER_TARGET_D3D11_H
#define _RENDER_TARGET_D3D11_H

#include <graphics/render_target.h>
#include <d3d11.h>

namespace gef
{
	class TextureVita;

	class RenderTargetD3D11 : public RenderTarget
	{
	public:
		RenderTargetD3D11(const Platform& platform, const Int32 width, const Int32 height);
		~RenderTargetD3D11();

		void Begin(const Platform& platform);
		void End(const Platform& platform);

		ID3D11RenderTargetView* render_target_view() { return render_target_view_; }

	private:
		void CleanUp();

		ID3D11RenderTargetView* render_target_view_;

	};
}



#endif // _RENDER_TARGET_D3D11_H