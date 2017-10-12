#include <maths/vector2.h>
#include <math.h>

namespace gef
{
	const Vector2 Vector2::kZero(0.0f, 0.0f);
	const Vector2 Vector2::kOne(1.0f, 1.0f);


	void Vector2::Normalise()
	{
		float length = Length();

		x /= length;
		y /= length;
	}

	float Vector2::LengthSqr() const
	{
		return (x*x + y*y);
	}

	float Vector2::Length() const
	{
		return sqrtf(x*x + y*y);
	}

	Vector2 Vector2::Rotate(float angle)
	{
		Vector2 result;

		result.x = x*cosf(angle) - y*sinf(angle);
		result.y = x*sinf(angle) + y*cosf(angle);

		return result;
	}

	float Vector2::DotProduct(const Vector2& _vec) const
	{
		return x*_vec.x + y*_vec.y;
	}

}