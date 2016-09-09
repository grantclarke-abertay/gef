#ifndef _GEF_PLATFORM_H
#define _GEF_PLATFORM_H

#include <gef.h>
#include <graphics/colour.h>
#include <string>
#include <list>
#include <maths/matrix44.h>

namespace gef
{
	// forward declarations
	class File;
	class SpriteRenderer;
	class AudioManager;
	class TouchInputManager;
	class Texture;
	class ImageData;
	class Mesh;
	class InputManager;
	class Renderer3D;
	class RenderTarget;
	class Shader;
	class VertexBuffer;
	class IndexBuffer;
	class ShaderInterface;
	class DepthBuffer;

	class Platform
	{
	public:
		Platform();
		virtual ~Platform();

		virtual bool Update() = 0;
		virtual float GetFrameTime() = 0;
		virtual void PreRender() = 0;
		virtual void PostRender() = 0;
		virtual void Clear() const= 0;
        //virtual void Release() = 0;

		std::string FormatFilename(const std::string& filename) const;
		virtual std::string FormatFilename(const char* filename) const = 0;
		virtual SpriteRenderer* CreateSpriteRenderer() = 0;
		virtual File* CreateFile() const = 0;
		virtual AudioManager* CreateAudioManager() const = 0;
		virtual InputManager* CreateInputManager() = 0;
//		virtual TouchInputManager* CreateTouchInputManager() const = 0;
//		virtual void InitTouchInputManager() = 0;
//		virtual void ReleaseTouchInputManager() = 0;
		virtual Texture* CreateTexture(const ImageData& image_data) const = 0;
		virtual Mesh* CreateMesh() = 0;
//		virtual SonyControllerInputManager* CreateSonyControllerInputManager() const = 0;
		virtual Renderer3D* CreateRenderer3D() = 0;
		virtual VertexBuffer* CreateVertexBuffer() const = 0;
		virtual IndexBuffer* CreateIndexBuffer() const = 0;
		virtual ShaderInterface* CreateShaderInterface() const = 0;

		void AddShader(Shader* shader);
		void RemoveShader(Shader* shader);
		void AddTexture(Texture* texture);
		void RemoveTexture(Texture* texture);
		void AddVertexBuffer(VertexBuffer* vertex_buffer);
		void RemoveVertexBuffer(VertexBuffer* vertex_buffer);
		void AddIndexBuffer(IndexBuffer* index_buffer);
		void RemoveIndexBuffer(IndexBuffer* index_buffer);


		/// Can only create render targets that are the same size as the screen at the moment
		/// so we can use the same depth buffer for all render targets
		RenderTarget* CreateRenderTarget() const;
		
		
		// going to test different sized render target
		// could just be the fact the depth buffer size has to be equal or bigger
		virtual RenderTarget* CreateRenderTarget(const Int32 width, const Int32 height) const = 0;

		virtual Matrix44 PerspectiveProjectionFov(const float fov, const float aspect_ratio, const float near_distance, const float far_distance) const = 0;
		virtual Matrix44 PerspectiveProjectionFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const = 0;
		virtual Matrix44 OrthographicFrustum(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance) const = 0;

		virtual void BeginScene() const = 0;
		virtual void EndScene() const = 0;
		virtual const char* GetShaderDirectory() const = 0;
		virtual const char* GetShaderFileExtension() const = 0;

		// check to see if the platform device is ready to render
		// this may be false if application focus has been lost.
		// e.g. android devices (phones, tablets, etc.)
		virtual bool ReadyToRender() const;

		virtual DepthBuffer* CreateDepthBuffer(UInt32 width, UInt32 height) const = 0;

		inline Int32 width() const { return width_; }
		inline Int32 height() const { return height_; }
		inline const Colour& render_target_clear_colour() const { return render_target_clear_colour_; }
		inline void set_render_target_clear_colour(const Colour& colour) { render_target_clear_colour_ = colour; }
		inline float depth_clear_value() const { return depth_clear_value_; }
		inline void set_depth_clear_value(const float value) { depth_clear_value_ = value; }
		inline UInt32 stencil_clear_value() const { return stencil_clear_value_; }
		inline void set_stencil_clear_value(const UInt32 value) { stencil_clear_value_ = value; }
		inline void set_render_target(RenderTarget* render_target) { render_target_ = render_target; }
		inline RenderTarget* render_target() const { return render_target_; }
		inline void set_touch_input_manager(TouchInputManager* touch_input_manager) { touch_input_manager_ = touch_input_manager; }
		inline TouchInputManager* touch_input_manager() const { return touch_input_manager_; }
		inline const Texture* default_texture() const { return default_texture_; }
		inline void set_depth_buffer(DepthBuffer* depth_buffer) { depth_buffer_ = depth_buffer; }
		inline DepthBuffer* depth_buffer() const { return depth_buffer_; }

	protected:
		inline void set_width(const Int32 width) { width_ = width; }
		inline void set_height(const Int32 height) { height_ = height; }




		Int32 width_;
		Int32 height_;
		Colour render_target_clear_colour_;
		float depth_clear_value_;
		UInt32 stencil_clear_value_;
		RenderTarget* render_target_;
		DepthBuffer* depth_buffer_;
		std::list<Shader*> shaders_;
		std::list<Texture*> textures_;
		std::list<VertexBuffer*> vertex_buffers_;
		std::list<IndexBuffer*> index_buffers_;
		TouchInputManager* touch_input_manager_;

		gef::Texture* default_texture_;
	};
}

#endif // _GEF_PLATFORM_H
