namespace gef
{

inline Vector4::Vector4()
{
}

inline Vector4::Vector4(const float new_x, const float new_y, const float new_z, const float new_w)
{
	values_[0] = new_x;
	values_[1] = new_y;
	values_[2] = new_z;
	values_[3] = new_w;
}

inline float Vector4::x() const
{
	return values_[0];
}

inline float Vector4::y() const
{
	return values_[1];
}

inline float Vector4::z() const
{
	return values_[2];
}

inline float Vector4::w() const
{
	return values_[3];
}

inline void Vector4::set_x(float x)
{
	values_[0] = x;
}

inline void Vector4::set_y(float y)
{
	values_[1] = y;
}

inline void Vector4::set_z(float z)
{
	values_[2] = z;
}

inline void Vector4::set_w(float w)
{
	values_[3] = w;
}

inline const float Vector4::operator[] (const int index) const
{
	return values_[index];
}

}