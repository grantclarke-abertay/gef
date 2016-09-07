#include <maths/plane.h>
#include <math.h>

namespace gef
{
	Plane::Plane(float a, float b, float c, float d) :
		Vector4(a, b, c, d)
	{

	}

	void Plane::Normalise()
	{
		float distance = sqrtf(a()*a() + b()*b() + c()*c());
		set_a(a() / distance);
		set_b(b() / distance);
		set_c(c() / distance);
		set_d(d() / distance);
	}
}