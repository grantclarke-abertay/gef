namespace gef
{

	inline Vector3::Vector3()
	{
	}

	inline Vector3::~Vector3()
	{
	}

	inline Vector3::Vector3(const float new_x, const float new_y, const float new_z)
	{
		values_[0] = new_x;
		values_[1] = new_y;
		values_[2] = new_z;
		values_[3] = 0.0f;
	}

	inline const Vector3 Vector3::operator-(const Vector3& _vec) const
	{
		Vector3 result;

		result.set_x(values_[0] - _vec.x());
		result.set_y(values_[1] - _vec.y());
		result.set_z(values_[2] - _vec.z());

		return result;
	}

	inline const Vector3 Vector3::operator+(const Vector3& _vec) const
	{
		Vector3 result;

		result.set_x(values_[0] + _vec.x());
		result.set_y(values_[1] + _vec.y());
		result.set_z(values_[2] + _vec.z());

		return result;
	}

	inline Vector3& Vector3::operator+=(const Vector3& _vec)
	{
		values_[0] += _vec.x();
		values_[1] += _vec.y();
		values_[2] += _vec.z();

		return *this;
	}

	inline Vector3& Vector3::operator-=(const Vector3& _vec)
	{
		values_[0] -= _vec.x();
		values_[1] -= _vec.y();
		values_[2] -= _vec.z();

		return *this;
	}

	inline const Vector3 Vector3::operator*(const float _scalar) const
	{
		Vector3 result;

		result.set_x(values_[0] * _scalar);
		result.set_y(values_[1] * _scalar);
		result.set_z(values_[2] * _scalar);

		return result;
	}

	inline const Vector3 Vector3::operator/(const float _scalar) const
	{
		Vector3 result;

		result.set_x(values_[0] / _scalar);
		result.set_y(values_[1] / _scalar);
		result.set_z(values_[2] / _scalar);

		return result;
	}

	inline Vector3& Vector3::operator*=(const float _scalar)
	{
		values_[0] *= _scalar;
		values_[1] *= _scalar;
		values_[2] *= _scalar;

		return *this;
	}

	inline Vector3& Vector3::operator/=(const float _scalar)
	{
		values_[0] /= _scalar;
		values_[1] /= _scalar;
		values_[2] /= _scalar;

		return *this;
	}

	inline const float Vector3::operator[] (const int index) const
	{
		return values_[index];
	}

	inline float Vector3::x() const
	{
		return values_[0];
	}

	inline float Vector3::y() const
	{
		return values_[1];
	}

	inline float Vector3::z() const
	{
		return values_[2];
	}

	inline void Vector3::set_x(float x)
	{
		values_[0] = x;
	}

	inline void Vector3::set_y(float y)
	{
		values_[1] = y;
	}

	inline void Vector3::set_z(float z)
	{
		values_[2] = z;
	}

	inline void Vector3::set_value(float x, float y, float z)
	{
		values_[0] = x;
		values_[1] = y;
		values_[2] = z;
		values_[3] = 0.0f;
	}
}