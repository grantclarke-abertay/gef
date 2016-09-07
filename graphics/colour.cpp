#include <graphics/colour.h>

namespace gef
{
	Colour::Colour()
	{
	}

	Colour::Colour(const float new_r, const float new_g, const float new_b, const float new_a) :
		r(new_r),
		g(new_g),
		b(new_b),
		a(new_a)
	{
	}


	
	UInt32 Colour::GetRGBA() const
	{
		return
			(static_cast<UInt32>(a*255) & 0x000000ff) |
			((static_cast<UInt32>(b*255) << 8) & 0x0000ff00) |
			((static_cast<UInt32>(g*255) << 16) & 0x00ff0000) |
			((static_cast<UInt32>(r*255) << 24) & 0xff000000);
	}

	UInt32 Colour::GetABGR() const
	{
		return
			(static_cast<UInt32>(r*255) & 0x000000ff) |
			((static_cast<UInt32>(g*255) << 8) & 0x0000ff00) |
			((static_cast<UInt32>(b*255) << 16) & 0x00ff0000) |
			((static_cast<UInt32>(a*255) << 24) & 0xff000000);
	}

	void Colour::SetFromRGBA(const UInt32 rgba)
	{
		const float coefficient = 1.f / 255.f;
		a =static_cast<float>(rgba & 0x000000ff)*coefficient;
		b = static_cast<float>((rgba >> 8) & 0x000000ff)*coefficient;
		g = static_cast<float>((rgba >> 16) & 0x000000ff)*coefficient;
		r = static_cast<float>((rgba >> 24) & 0x000000ff)*coefficient;
	}

	void Colour::SetFromAGBR(const UInt32 rgba)
	{
		float coefficient = 1.f / 255.f;
		r = static_cast<float>(rgba & 0x000000ff)*coefficient;
		g = static_cast<float>((rgba >> 8) & 0x000000ff)*coefficient;
		b = static_cast<float>((rgba >> 16) & 0x000000ff)*coefficient;
		a = static_cast<float>((rgba >> 24) & 0x000000ff)*coefficient;
	}
}