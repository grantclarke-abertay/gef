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

inline float Vector4::w() const
{
	return values_[3];
}

inline void Vector4::set_w(float w)
{
	values_[3] = w;
}

inline void Vector4::set_value(float x, float y, float z, float w)
{
	values_[0] = x;
	values_[1] = y;
	values_[2] = z;
	values_[3] = w;
}


}