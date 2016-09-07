#ifndef _GEF_COLOUR_H
#define _GEF_COLOUR_H

#include <gef.h>
#include <maths/vector4.h>

namespace gef
{
	class Colour
	{
	public:
		Colour();
		Colour(const float r, const float g, const float b, const float a = 1.0f);

		UInt32 GetRGBA() const;
		UInt32 GetABGR() const;
		inline Vector4 GetRGBAasVector4() const { return Vector4(r, g, b, a); }
		inline Vector4 GetABGRasVector4() const { return Vector4(a, b, g, r); }
		void SetFromRGBA(const UInt32 rgba);
		void SetFromAGBR(const UInt32 rgba);

		float r;
		float g;
		float b;
		float a;
	};
}

#endif // _GEF_COLOUR_H
