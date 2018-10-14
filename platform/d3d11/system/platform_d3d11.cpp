#include <platform/d3d11/system/platform_d3d11.h>
#include <platform/win32/system/window_win32.h>
#include <platform/win32/system/file_win32.h>
#include <platform/d3d11/graphics/texture_d3d11.h>
#include <platform/d3d11/graphics/sprite_renderer_d3d11.h>
#include <platform/d3d11/input/touch_input_manager_d3d11.h>
#include <platform/d3d11/input/input_manager_d3d11.h>
#include <graphics/mesh.h>
#include <platform/d3d11/graphics/renderer_3d_d3d11.h>
#include <platform/d3d11/graphics/render_target_d3d11.h>
#include <platform/d3d11/graphics/vertex_buffer_d3d11.h>
#include <platform/d3d11/graphics/index_buffer_d3d11.h>
#include <platform/d3d11/graphics/shader_interface_d3d11.h>
#include <platform/d3d11/graphics/depth_buffer_d3d11.h>
#include <cassert>


namespace gef
{
	PlatformD3D11::PlatformD3D11(HINSTANCE hinstance, UInt32 width, UInt32 height, bool fullscreen, bool vsync_enabled, HWND hwnd) :
		window_(NULL),
		clock_last_frame_(0),
		device_(NULL),
		device_context_(NULL),
		swap_chain_(NULL),
		back_buffer_(NULL),
		render_target_view_(NULL),
		depth_stencil_(NULL),
		depth_stencil_view_(NULL),
		vsync_enabled_(vsync_enabled),
		hwnd_(NULL),
		top_level_hwnd_(NULL),
		screenshot_texture_(NULL)
	{
		HRESULT hresult = S_OK;

		set_render_target_clear_colour(Colour(0.4f, 0.525f, 0.7f, 1.0f)); 
		set_depth_clear_value(1.0f);
		set_stencil_clear_value(0);

		// create window if window has not already been provided
		if(hwnd == NULL)
		{
			window_ = new WindowWin32(hinstance, width, height, fullscreen, NULL);
			set_width(window_->width());
			set_height(window_->height());
			hwnd_ = window_->hwnd();
		}
		else
		{
			set_width(width);
			set_height(height);
			hwnd_ = hwnd;
		}
		// initialise top level wnd the same as the rendering hwnd
		// top level wnd can be overriden externally
		top_level_hwnd_ = hwnd_;



		// Store the vsync setting.
		vsync_enabled_ = vsync_enabled;



		bool success = true;
		success = InitDeviceAndSwapChain(fullscreen);

		if(success)
		{
			// Create a render target view
			hresult = swap_chain_->GetBuffer(0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&back_buffer_);

			if(SUCCEEDED(hresult))
			{
				hresult = device_->CreateRenderTargetView( back_buffer_, NULL, &render_target_view_ );
			}

			// Create DepthStencil Buffer
			if(SUCCEEDED(hresult))
			{
				hresult = CreateDepthStencilBuffer();
			}
		}

		if(SUCCEEDED(hresult))
		{
			// bind the render and depth targets to the rendering pipeline 
			device_context_->OMSetRenderTargets( 1, &render_target_view_, depth_stencil_view_ );

			// Setup the viewport
			SetupViewport();
		}
		else
		{
			Release();
		}

		// create default texture
		default_texture_ = Texture::CreateCheckerTexture(16, 1, *this);
		AddTexture(default_texture_);
	}

	PlatformD3D11::~PlatformD3D11()
	{
		Release();
	}

	void PlatformD3D11::Release()
	{
		if (default_texture_)
		{
			RemoveTexture(default_texture_);
			delete default_texture_;
			default_texture_ = NULL;
		}

		ReleaseNull(screenshot_texture_);
		ReleaseNull(depth_stencil_view_);
		ReleaseNull(depth_stencil_);
		ReleaseNull(render_target_view_);
		ReleaseNull(back_buffer_);
		ReleaseDeviceAndSwapChain();
		DeleteNull(window_);
	}

	bool PlatformD3D11::InitDeviceAndSwapChain(bool fullscreen)
	{
		IDXGIFactory* factory = NULL;
		IDXGIAdapter* adapter = NULL;
		IDXGIOutput* adapter_output = NULL;
		HRESULT hresult = S_OK;

		// Determine the resolution of the clients desktop screen.
		UInt32 screen_width  = GetSystemMetrics(SM_CXSCREEN);
		UInt32 screen_height = GetSystemMetrics(SM_CYSCREEN);

		UInt32 numerator = 0;
		UInt32 denominator = 1;

		// Create a DirectX graphics interface factory.
		hresult = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if(SUCCEEDED(hresult))
		{
			// Use the factory to create an adapter for the primary graphics interface (video card).
			hresult = factory->EnumAdapters(0, &adapter);
		}


		// Enumerate the primary adapter output (monitor).
		if(SUCCEEDED(hresult))
			hresult = adapter->EnumOutputs(0, &adapter_output);

		// Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
		UInt32 num_modes = 0;
		if(SUCCEEDED(hresult))
			hresult = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, NULL);

		if(SUCCEEDED(hresult))
		{
			// Create a list to hold all the possible display modes for this monitor/video card combination.
			DXGI_MODE_DESC* display_mode_list = new DXGI_MODE_DESC[num_modes];

			// Now fill the display mode list structures.
			hresult = adapter_output->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &num_modes, display_mode_list);
			if(SUCCEEDED(hresult))
			{	
				// Now go through all the display modes and find the one that matches the screen width and height.
				// When a match is found store the numerator and denominator of the refresh rate for that monitor.
				for(UInt32 i=0; i<num_modes; i++)
				{
					if(display_mode_list[i].Width == (unsigned int)screen_width)
					{
						if(display_mode_list[i].Height == (unsigned int)screen_height)
						{
							numerator = display_mode_list[i].RefreshRate.Numerator;
							denominator = display_mode_list[i].RefreshRate.Denominator;
						}
					}
				}
			}
			// Release the display mode list.
			delete [] display_mode_list;
			display_mode_list = 0;
		}

		// Release the adapter output.
		ReleaseNull(adapter_output);
		ReleaseNull(adapter);
		ReleaseNull(factory);

		UINT create_device_flags = 0;
	#ifdef _DEBUG
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

		D3D_DRIVER_TYPE driver_types[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT num_driver_types = sizeof( driver_types ) / sizeof( driver_types[0] );

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory( &sd, sizeof( sd ) );
		sd.BufferCount = 1;
		sd.BufferDesc.Width = width();
		sd.BufferDesc.Height = height();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if(vsync_enabled_)
		{
			sd.BufferDesc.RefreshRate.Numerator = numerator;
			sd.BufferDesc.RefreshRate.Denominator = denominator;
		}
		else
		{
			sd.BufferDesc.RefreshRate.Numerator = 0;
			sd.BufferDesc.RefreshRate.Denominator = 1;
		}
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd_;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = fullscreen ? FALSE : TRUE;

		// Set the feature level to DirectX 11.
		D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT num_feature_levels = sizeof( feature_levels ) / sizeof( feature_levels[0] );
	
		for( UINT driver_type_index = 0; driver_type_index < num_driver_types; driver_type_index++ )
		{
			driver_type_ = driver_types[driver_type_index];
			hresult = D3D11CreateDeviceAndSwapChain( NULL, this->driver_type_, NULL, create_device_flags, feature_levels, num_feature_levels,
				D3D11_SDK_VERSION, &sd, &swap_chain_, &device_, NULL, &device_context_);
			if( SUCCEEDED( hresult ) )
				break;
		}
    
		if SUCCEEDED(hresult)
		{
			return true;
		}
		else
		{
			Release();
			return false;
		}
	}

	void PlatformD3D11::ReleaseDeviceAndSwapChain()
	{
		if( device_context_ )
			device_context_->ClearState();

		ReleaseNull(device_context_);

		// Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
		if(swap_chain_)
			swap_chain_->SetFullscreenState(false, NULL);

		ReleaseNull(swap_chain_);
#ifdef _DEBUG
		if (0)
		{
			ID3D11Debug* debug;
			device_->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&debug));
			debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
			debug->Release();
			debug = NULL;
		}
#endif
		ReleaseNull(device_);
	}

	HRESULT PlatformD3D11::CreateDepthStencilBuffer()
	{
		HRESULT hresult = S_OK;
		D3D11_TEXTURE2D_DESC desc_depth;
		desc_depth.Width = width_;
		desc_depth.Height = height_;
		desc_depth.MipLevels = 1;
		desc_depth.ArraySize = 1;
		desc_depth.Format = DXGI_FORMAT_D32_FLOAT;
		desc_depth.SampleDesc.Count = 1;
		desc_depth.SampleDesc.Quality = 0;
		desc_depth.Usage = D3D11_USAGE_DEFAULT;
		desc_depth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		desc_depth.CPUAccessFlags = 0;
		desc_depth.MiscFlags = 0;
		hresult = device_->CreateTexture2D( &desc_depth, NULL, &depth_stencil_ );

		if(SUCCEEDED(hresult))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC desc_DSV;
			ZeroMemory(&desc_DSV, sizeof(desc_DSV));
			desc_DSV.Format = desc_depth.Format;
			desc_DSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			desc_DSV.Texture2D.MipSlice = 0;
			hresult = device_->CreateDepthStencilView(depth_stencil_, &desc_DSV, &this->depth_stencil_view_ );
		}

		return hresult;
	}

	void PlatformD3D11::SetupViewport() const
	{
		if (render_target())
		{

			D3D11_VIEWPORT vp;
			vp.Width = static_cast<FLOAT>(render_target()->width());
			vp.Height = static_cast<FLOAT>(render_target()->height());
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			device_context_->RSSetViewports(1, &vp);
		}
		else
		{
			D3D11_VIEWPORT vp;
			vp.Width = static_cast<FLOAT>(width_);
			vp.Height = static_cast<FLOAT>(height_);
			vp.MinDepth = 0.0f;
			vp.MaxDepth = 1.0f;
			vp.TopLeftX = 0;
			vp.TopLeftY = 0;
			device_context_->RSSetViewports(1, &vp);
		}
	}

	void PlatformD3D11::Resize(UInt32 width, UInt32 height)
	{
		HRESULT hr = S_OK;
		//Platform::Resize(width, height);
		set_width(width);
		set_height(height);

		ID3D11Resource *pBackBufferResource = NULL;
		ID3D11RenderTargetView *viewList[1] = {NULL};

		device_context_->ClearState();
		device_context_->OMSetRenderTargets(1, viewList, NULL);

		// Ensure that nobody is holding onto one of the old resources
		ReleaseNull(render_target_view_);
		ReleaseNull(depth_stencil_view_);

		// Resize render target buffers
		ReleaseNull(back_buffer_);
		hr = swap_chain_->ResizeBuffers(1, width, height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);


		if (SUCCEEDED(hr))
		{
			// Create a render target view
			hr = swap_chain_->GetBuffer(0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&back_buffer_);

			if (SUCCEEDED(hr))
				hr = device_->CreateRenderTargetView(back_buffer_, NULL, &render_target_view_);
		}

		if (SUCCEEDED(hr))
		{
			hr = CreateDepthStencilBuffer();
		}

		if (SUCCEEDED(hr))
		{
			// bind the render and depth targets to the rendering pipeline 
			device_context_->OMSetRenderTargets( 1, &render_target_view_, depth_stencil_view_ );

			// Set a new viewport based on the new dimensions
			SetupViewport();
		}
	}


	void PlatformD3D11::PreRender()
	{
	//    float clear_colour[4] = { 0.4f, 0.525f, 0.7f, 1.0f }; //red,green,blue,alpha
	//	device_context_->ClearRenderTargetView( render_target_view_, clear_colour );
	//	device_context_->ClearDepthStencilView( depth_stencil_view_, D3D11_CLEAR_DEPTH, 1.0f, 0 );
		//Clear(true, true, true);
	}

	void PlatformD3D11::PostRender()
	{
		if(swap_chain_)
			swap_chain_->Present(vsync_enabled_ ? 1 : 0, 0);
	}

	void PlatformD3D11::Clear() const
	{
		Clear(true, true, true);
	}

	void PlatformD3D11::Clear(const bool clear_render_target_enabled, const bool clear_depth_buffer_enabled, const bool clear_stencil_buffer_enabled ) const
	{
		ID3D11RenderTargetView* render_target_view = GetRenderTargetView();

		if(clear_render_target_enabled)
			device_context_->ClearRenderTargetView( render_target_view, reinterpret_cast<const float*>(&render_target_clear_colour()) );

		UInt32 clear_flags = 0;
		if(clear_depth_buffer_enabled)
			clear_flags |= D3D11_CLEAR_DEPTH;

		if(clear_stencil_buffer_enabled)
			clear_flags |= D3D11_CLEAR_STENCIL;

		// if any of the flags have been set then
		// make the call to ClearRenderTargetView
		if(clear_flags)
		{
			ID3D11DepthStencilView* depth_stencil_view = GetDepthStencilView();
			device_context_->ClearDepthStencilView(depth_stencil_view, clear_flags, depth_clear_value(), stencil_clear_value());
		}
	}

	bool PlatformD3D11::Update()
	{
		if(touch_input_manager_)
			touch_input_manager_->CleanupReleasedTouches();

		// Initialize the message structure.
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		// Handle the windows messages.
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return msg.message != WM_QUIT;
	}

	float PlatformD3D11::GetFrameTime()
	{
		// calculate the time between updates
		UInt64 clock, clock_frequency;
		if(clock_last_frame_ == 0)
			QueryPerformanceCounter((LARGE_INTEGER*)&clock_last_frame_);

		QueryPerformanceCounter((LARGE_INTEGER*)&clock);
		QueryPerformanceFrequency((LARGE_INTEGER*)&clock_frequency);
		UInt32 cycles = (UInt32)(clock - clock_last_frame_);
		clock_last_frame_ = clock;

		return (float)cycles / (float)clock_frequency;
	}

	std::string PlatformD3D11::FormatFilename(const std::string& filename) const
	{
		return filename;
	}

	std::string PlatformD3D11::FormatFilename(const char* filename) const
	{
		return std::string(filename);
	}


	Matrix44 PlatformD3D11::PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.PerspectiveFovD3D(fov, aspect_ratio, near_distance, far_distance);
		return projection_matrix;
	}

	Matrix44 PlatformD3D11::PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.PerspectiveFrustumD3D(left, right, top, bottom, near_distance, far_distance);
		return projection_matrix;
	}

	Matrix44 PlatformD3D11::OrthographicFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const
	{
		Matrix44 projection_matrix;
		projection_matrix.OrthographicFrustumD3D(left, right, top, bottom, near_distance, far_distance);
		return projection_matrix;
	}

	void PlatformD3D11::BeginScene() const
	{
		ID3D11RenderTargetView* render_target_view = GetRenderTargetView();
		ID3D11DepthStencilView* depth_stencil_view = GetDepthStencilView();
		device_context_->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);
		SetupViewport();
	}

	void PlatformD3D11::EndScene() const
	{
	}

	ID3D11RenderTargetView* PlatformD3D11::GetRenderTargetView() const
	{
		if(render_target())
		{
			RenderTargetD3D11* render_target_d3d11 = static_cast<RenderTargetD3D11*>(render_target());
			return render_target_d3d11->render_target_view();
		}
		else
		{
			return render_target_view_;
		}
	}

	ID3D11DepthStencilView* PlatformD3D11::GetDepthStencilView() const
	{
		if (depth_buffer())
		{
			DepthBufferD3D11* depth_buffer_d3d11 = static_cast<DepthBufferD3D11*>(depth_buffer());
			return depth_buffer_d3d11->depth_stencil_view();
		}
		else
		{
			return depth_stencil_view_;
		}
	}


	const char* PlatformD3D11::GetShaderDirectory() const
	{
		return "d3d11";
	}

	const char* PlatformD3D11::GetShaderFileExtension() const
	{
		return "hlsl";
	}


	class SpriteRenderer* PlatformD3D11::CreateSpriteRenderer()
	{
		return new SpriteRendererD3D11(*this);
	}

	InputManager* PlatformD3D11::CreateInputManager()
	{
		return new InputManagerD3D11(*this);
	}

	Renderer3D* PlatformD3D11::CreateRenderer3D()
	{
		return new Renderer3DD3D11(*this);
	}

}





