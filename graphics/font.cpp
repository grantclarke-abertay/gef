#include <graphics/font.h>
#include <maths/vector2.h>
#include <graphics/texture.h>
#include <graphics/sprite_renderer.h>
#include <graphics/sprite.h>
#include <assets/png_loader.h>
#include <graphics/image_data.h>
#include <system/platform.h>
#include <system/file.h>
#include <system/memory_stream_buffer.h>
//#include <fstream>
#include <sstream>
#include <cstdarg>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>



namespace gef
{

Font::Font(Platform& platform) :
font_texture_(NULL),
	platform_(platform)
{
}

Font::~Font()
{
	if(font_texture_)
	{
		platform_.RemoveTexture(font_texture_);
		delete font_texture_;
		font_texture_ = NULL;
	}
}

bool Font::Load(const char* font_name)
{
	std::string font_config_filename(font_name);
	font_config_filename += ".fnt";
	void* font_file_data = NULL;
	Int32 file_size = 0;
	gef::File* file = gef::File::Create();
	
	bool success = true;
	success = file->Open(font_config_filename.c_str());
	if(success)
	{
		success = file->GetSize(file_size);
		if(success)
		{
			font_file_data = malloc(file_size);
			success = font_file_data != NULL;
			if(success)
			{
				Int32 bytes_read;
				success = file->Read(font_file_data, file_size, bytes_read);
				if(success)
					success = bytes_read == file_size;
			}
		}
		file->Close();
	}

	delete file;
	file = NULL;




	bool config_initialised = false;

	if(success)
	{
		gef::MemoryStreamBuffer font_buffer((char*)font_file_data, file_size);

		std::istream font_config_stream(&font_buffer);
		config_initialised = ParseFont(font_config_stream, character_set);

		// don't need the font file data any more
		free(font_file_data);
		font_file_data = NULL;

		std::string font_texture_filename(font_name);
		font_texture_filename += "_0.png";
		PNGLoader png_loader;
		gef::ImageData image_data;
		png_loader.Load(font_texture_filename.c_str(), platform_, image_data);
		font_texture_ = gef::Texture::Create(platform_, image_data);
		platform_.AddTexture(font_texture_);
	}

	return config_initialised;
}


bool Font::ParseFont( std::istream& Stream, Font::Charset& CharsetDesc )
{
	std::string Line;
	std::string Read, Key, Value;
	std::size_t i;
	while( !Stream.eof() )
	{
		std::stringstream LineStream;
		std::getline( Stream, Line );
		LineStream << Line;

		//read the line's type
		LineStream >> Read;
		if( Read == "common" )
		{
			//this holds common data
			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "lineHeight" )
					Converter >> CharsetDesc.LineHeight;
				else if( Key == "base" )
					Converter >> CharsetDesc.Base;
				else if( Key == "scaleW" )
					Converter >> CharsetDesc.Width;
				else if( Key == "scaleH" )
					Converter >> CharsetDesc.Height;
				else if( Key == "pages" )
					Converter >> CharsetDesc.Pages;
			}
		}
		else if( Read == "char" )
		{
			//this is data for a specific char
			unsigned short CharID = 0;

			while( !LineStream.eof() )
			{
				std::stringstream Converter;
				LineStream >> Read;
				i = Read.find( '=' );
				Key = Read.substr( 0, i );
				Value = Read.substr( i + 1 );

				//assign the correct value
				Converter << Value;
				if( Key == "id" )
					Converter >> CharID;
				else if( Key == "x" )
					Converter >> CharsetDesc.Chars[CharID].x;
				else if( Key == "y" )
					Converter >> CharsetDesc.Chars[CharID].y;
				else if( Key == "width" )
					Converter >> CharsetDesc.Chars[CharID].Width;
				else if( Key == "height" )
					Converter >> CharsetDesc.Chars[CharID].Height;
				else if( Key == "xoffset" )
					Converter >> CharsetDesc.Chars[CharID].XOffset;
				else if( Key == "yoffset" )
					Converter >> CharsetDesc.Chars[CharID].YOffset;
				else if( Key == "xadvance" )
					Converter >> CharsetDesc.Chars[CharID].XAdvance;
				else if( Key == "page" )
					Converter >> CharsetDesc.Chars[CharID].Page;
			}
		}
	}

	return true;
}

void Font::RenderText(SpriteRenderer* renderer, const class Vector4& pos, const float scale, const UInt32 colour, const TextJustification justification, const char* text, ...) const
{
	if(!text)
		return;

	va_list args;
	char text_buffer[256];

	va_start(args, text);
	std::vsprintf(text_buffer, text, args);

	UInt32 character_count = (UInt32)strlen(text_buffer);
	float string_length = GetStringLength(text_buffer);

	Vector2 cursor = Vector2(pos.x(), pos.y());

	switch(justification)
	{
	case TJ_CENTRE:
		cursor.x -= string_length*0.5f*scale;
		break;
	case TJ_RIGHT:
		cursor.x -= string_length*scale;
		break;
	default:
		break;
	}

	Sprite sprite;
	sprite.set_texture(font_texture_);
	for (UInt32 character_index = 0; character_index < character_count; ++character_index)
	{
		Int32 CharX = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].x;
		Int32 CharY = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].y;
		Int32 Width = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].Width;
		Int32 Height = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].Height;
		Int32 OffsetX = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].XOffset;
		Int32 OffsetY = character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].YOffset;

		Vector2 uv_pos((float) CharX / (float) character_set.Width,  ((float) (CharY) / (float) character_set.Height));
		Vector2 uv_size((float) (Width) / (float) character_set.Width, (float)(Height) / (float) character_set.Height);
		Vector2 size(((float)Width)*scale, ((float)Height)*scale);
		Vector4 sprite_position = Vector4(cursor.x+((float)OffsetX*scale)+size.x*0.5f, cursor.y + scale*((float)Height*0.5f +  (float)OffsetY), pos.z());

		sprite.set_position(sprite_position);
		sprite.set_width(size.x);
		sprite.set_height(size.y);
		sprite.set_uv_position(uv_pos);
		sprite.set_uv_width(uv_size.x);
		sprite.set_uv_height(uv_size.y);
		sprite.set_colour(colour);
		renderer->DrawSprite(sprite);
		cursor.x += ((float)character_set.Chars[static_cast<UInt32>(text_buffer[character_index])].XAdvance)*scale;
	}
}

float Font::GetStringLength(const char * text) const
{
	float length = 0.0f;
	if(text)
	{
		UInt32 string_length = (UInt32)strlen(text);


		for( UInt32 character_index = 0; character_index < string_length; ++character_index )
			length += ((float)character_set.Chars[static_cast<UInt32>(text[character_index])].XAdvance);
	}

	return length;
}

}
