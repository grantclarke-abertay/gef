#ifndef _GEF_FONT_H
#define _GEF_FONT_H

#include <gef.h>
#include <istream>

namespace gef
{
	// FORWARD DECLARATIONS
	class SpriteRenderer;
	class Texture;
	class Platform;
	class Vector3;

	enum TextJustification
	{
		TJ_LEFT = 0,
		TJ_CENTRE,
		TJ_RIGHT,
	};

	class Font
	{
	public:
		Font(Platform& platform);
		~Font();
		bool Load(const char* font_name);
		void RenderText(SpriteRenderer* renderer, const Vector3& pos, const float scale, const UInt32 colour, const TextJustification justification, const char * text, ...) const;
		float GetStringLength(const char * text) const;

		inline Texture* font_texture() { return font_texture_; }
	private:
		struct CharDescriptor
		{
			//clean 16 bytes
			Int32 x, y;
			Int32 Width, Height;
			Int32 XOffset, YOffset;
			Int32 XAdvance;
			Int32 Page;

			CharDescriptor() : x( 0 ), y( 0 ), Width( 0 ), Height( 0 ), XOffset( 0 ), YOffset( 0 ),
				XAdvance( 0 ), Page( 0 )
			{ }
		};

		struct Charset
		{
			UInt16 LineHeight;
			UInt16 Base;
			UInt16 Width, Height;
			UInt16 Pages;
			CharDescriptor Chars[256];
		};

		bool ParseFont( std::istream& Stream, Font::Charset& CharsetDesc );

		Charset character_set;
		class Texture* font_texture_;

		Platform& platform_;
	};
}

#endif // _GEF_FONT_H
