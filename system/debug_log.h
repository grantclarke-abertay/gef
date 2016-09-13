#ifndef _GEF_DEBUG_LOG_H
#define _GEF_DEBUG_LOG_H

namespace gef
{
	class Matrix44;
	class Vector4;

	void DebugOut(const char* label, const Matrix44& matrix);
	void DebugOut(const char* label, const Vector4& vector);
	void DebugOut(const char * text, ...);
}

#endif // _GEF_DEBUG_LOG_H
