namespace gef
{

inline Vector2::Vector2()
{
}

inline Vector2::~Vector2()
{
}

inline Vector2::Vector2(const float new_x, const float new_y)
{
	x = new_x;
	y = new_y;
}

inline const Vector2 Vector2::operator-(const Vector2& vec) const
{
	Vector2 result;

	result.x = x-vec.x;
	result.y = y-vec.y;

	return result;
}

inline const Vector2 Vector2::operator+(const Vector2& vec) const
{
	Vector2 result;

	result.x = x+vec.x;
	result.y = y+vec.y;

	return result;
}

inline Vector2& Vector2::operator+=(const Vector2& vec)
{
	x += vec.x;
	y += vec.y;

	return *this;
}

inline Vector2& Vector2::operator-=(const Vector2& vec)
{
	x -= vec.x;
	y -= vec.y;

	return *this;
}

inline const Vector2 Vector2::operator*(const float scalar) const
{
	Vector2 result;

	result.x = x*scalar;
	result.y = y*scalar;

	return result;
}

inline const Vector2 Vector2::operator/(const float scalar) const
{
	Vector2 result;

	result.x = x/scalar;
	result.y = y/scalar;

	return result;
}

inline Vector2& Vector2::operator*=(const float scalar)
{
	x *= scalar;
	y *= scalar;

	return *this;
}

inline Vector2& Vector2::operator/=(const float scalar)
{
	x /= scalar;
	y /= scalar;

	return *this;
}

}