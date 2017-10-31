/*
 * debug_log_std.cpp
 *
 *  Created on: 29 Jan 2015
 *      Author: grant
 */

#include <system/debug_log.h>
#include <cstdarg>
#include <cstdio>
#include <iostream>

#include <maths/matrix44.h>
#include <maths/vector4.h>

namespace gef
{
	void DebugOut(const char * text, ...)
	{
		va_list args;
		char text_buffer[256];

		va_start(args, text);
		std::vsprintf(text_buffer, text, args);


		std::cout << text_buffer;
	}


	void DebugOut(const char* label, const Matrix44& matrix)
	{
		DebugOut("%s\n", label);
		for (int i = 0; i<4; ++i)
		{
			for(int j=0;j<4;++j)
				DebugOut("%f ", matrix.m(i,j));
			DebugOut("\n");
		}
	}

	void DebugOut(const char* label, const Vector4& vector)
	{
		DebugOut("%s: %f %f %f \n", label, vector.x(), vector.y(), vector.z());
	}
}

