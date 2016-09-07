namespace gef
{

inline Quaternion::Quaternion()
{
}

inline Quaternion::Quaternion(float new_x, float new_y, float new_z, float new_w) :
	x(new_x),
	y(new_y),
	z(new_z),
	w(new_w)
{
}




inline float Quaternion::LengthSquared() const
{
	return x*x + y*y + z*z +w*w;
}

inline float Quaternion::Length() const
{
	return sqrtf(LengthSquared());
}

inline const Quaternion Quaternion::operator -() const
{
	return Quaternion(-x, -y, -z, -w); 
}

inline const Quaternion Quaternion::operator *(float scale) const
{
	return Quaternion(scale*x, scale*y, scale*z, scale*w);
}

inline const Quaternion Quaternion::operator /(float scale) const
{
	return Quaternion(x/scale, y/scale, z/scale, w/scale);
}

inline const Quaternion Quaternion::operator +(const Quaternion &q) const	
{
	return Quaternion(q.x+x, q.y+y, q.z+z, q.w+w);
}

}
