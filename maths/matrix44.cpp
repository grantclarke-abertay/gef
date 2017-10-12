#include <maths/matrix44.h>
#include <maths/vector4.h>
#include <maths/vector4.h>
#include <maths/quaternion.h>
#include <math.h>


namespace gef
{

	Matrix44::Matrix44(const float *m)
	{
		values_[0] = Vector4(m[0], m[1], m[2], m[3]);
		values_[1] = Vector4(m[4], m[5], m[6], m[7]);
		values_[2] = Vector4(m[8], m[9], m[10], m[11]);
		values_[3] = Vector4(m[12], m[13], m[14], m[15]);

	}

	void Matrix44::SetIdentity()
	{
		values_[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
		values_[1] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
		values_[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
		values_[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	}

	void Matrix44::SetZero()
	{
		values_[0] = Vector4::kZero;
		values_[1] = Vector4::kZero;
		values_[2] = Vector4::kZero;
		values_[3] = Vector4::kZero;
	}

	const Matrix44 Matrix44::operator*(const Matrix44& matrix) const
	{
		Matrix44 result;

		for (int i = 0; i < 4; i++)
		{
			result.values_[i].set_x(values_[i].x() * matrix.values_[0].x() + values_[i].y() * matrix.values_[1].x() + values_[i].z() * matrix.values_[2].x() + values_[i].w() * matrix.values_[3].x());
			result.values_[i].set_y(values_[i].x() * matrix.values_[0].y() + values_[i].y() * matrix.values_[1].y() + values_[i].z() * matrix.values_[2].y() + values_[i].w() * matrix.values_[3].y());
			result.values_[i].set_z(values_[i].x() * matrix.values_[0].z() + values_[i].y() * matrix.values_[1].z() + values_[i].z() * matrix.values_[2].z() + values_[i].w() * matrix.values_[3].z());
			result.values_[i].set_w(values_[i].x() * matrix.values_[0].w() + values_[i].y() * matrix.values_[1].w() + values_[i].z() * matrix.values_[2].w() + values_[i].w() * matrix.values_[3].w());
		}

		return result; 
	}

	void Matrix44::LookAt(const Vector4& eye, const Vector4& lookat, const Vector4& up)
	{
		SetIdentity();
		Vector4 forward = eye - lookat;
		forward.Normalise();

		Vector4 side;
		side = up.CrossProduct(forward);
		side.Normalise();

		Vector4 calculated_up;
		calculated_up = forward.CrossProduct(side);

		values_[0].set_x(side.x());
		values_[1].set_x(side.y());
		values_[2].set_x(side.z());
		values_[0].set_y(calculated_up.x());
		values_[1].set_y(calculated_up.y());
		values_[2].set_y(calculated_up.z());
		values_[0].set_z(forward.x());
		values_[1].set_z(forward.y());
		values_[2].set_z(forward.z());
		values_[3].set_x(-side.DotProduct(eye));
		values_[3].set_y(-calculated_up.DotProduct(eye));
		values_[3].set_z(-forward.DotProduct(eye));
	}

	void Matrix44::PerspectiveFrustumGL(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance)
	{
		// transform to openGL clip space
		// cube dimensions (-1, -1, -1) to (1, 1, 1)
		float a, b, c, d;

		a = (right + left) / (right - left);
		b = (top + bottom) / (top - bottom);
		c = -(far_distance+near_distance) / (far_distance - near_distance);
		d = -(2.0f*far_distance*near_distance) / (far_distance - near_distance);

		SetZero();
		values_[0].set_x((2.0f*near_distance) / (right - left));
		values_[1].set_y((2.0f*near_distance) / (top - bottom));
		values_[2].set_x(a);
		values_[2].set_y(b);
		values_[2].set_z(c);
		values_[2].set_w(-1.0f);
		values_[3].set_z(d);
	}

	void Matrix44::PerspectiveFovGL(const float fov, const float aspect_ratio, const float near_distance, const float far_distance)
	{
		float height = tanf(fov*0.5f)*near_distance;
		float width = height*aspect_ratio;
		PerspectiveFrustumGL(-width, width, height, -height, near_distance, far_distance);
	}

	
	void Matrix44::OrthographicFrustumGL(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance)
	{
		// transform to openGL clip space
		// cube dimensions (-1, -1, -1) to (1, 1, 1)
		SetIdentity();
		values_[0].set_x((2.0f) / (right - left));
		values_[3].set_x(-(right+left) / (right - left));
		values_[1].set_y((2.0f) / (top - bottom));
		values_[3].set_y(-(top+bottom) / (top - bottom));
		values_[2].set_z((2.0f) / (far_distance - near_distance));
		values_[3].set_z(-(far_distance+near_distance) / (far_distance - near_distance));
	}

	void Matrix44::PerspectiveFrustumD3D(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance)
	{
		// transform to D3D clip space
		// cube dimensions (-1, -1, 0) to (1, 1, 1)

		float a, b, c, d;

		a = (right + left) / (right - left);
		b = (top + bottom) / (top - bottom);
		c = (far_distance) / (near_distance - far_distance);
		d = (far_distance*near_distance) / (near_distance - far_distance);

		SetZero();
		values_[0].set_x((2.0f*near_distance) / (right - left));
		values_[1].set_y((2.0f*near_distance) / (top - bottom));
		values_[2].set_x(a);
		values_[2].set_y(b);
		values_[2].set_z(c);
		values_[2].set_w(-1.0f);
		values_[3].set_z(d);
	}

	void Matrix44::PerspectiveFovD3D(const float fov, const float aspect_ratio, const float near_distance, const float far_distance)
	{
		float height = tanf(fov*0.5f)*near_distance;
		float width = height*aspect_ratio;
		PerspectiveFrustumD3D(-width, width, height, -height, near_distance, far_distance);
	}


	void Matrix44::OrthographicFrustumD3D(const float left, const float right, const float top, const float bottom, const float near_distance, const float far_distance)
	{
		// transform to D3D clip space
		// cube dimensions (-1, -1, 0) to (1, 1, 1)
		SetIdentity();
		values_[0].set_x((2.0f) / (right - left));
		values_[3].set_x((right + left) / (left - right));
		values_[1].set_y((2.0f) / (top - bottom));
		values_[3].set_y((top + bottom) / (bottom - top));
		values_[2].set_z((1.0f) / (far_distance - near_distance));
		values_[3].set_z((near_distance) / (near_distance - far_distance));
	}

	void Matrix44::RotationX(float _radiansRot)
	{
		values_[0].set_x(1.0f);
		values_[0].set_y(0.0f);
		values_[0].set_z(0.0f);
		values_[0].set_w(0.0f);

		values_[1].set_x(0.0f);
		values_[1].set_y(cosf(_radiansRot));
		values_[1].set_z(sinf(_radiansRot));
		values_[1].set_w(0.0f);

		values_[2].set_x(0.0f);
		values_[2].set_y(-sinf(_radiansRot));
		values_[2].set_z(cosf(_radiansRot));
		values_[2].set_w(0.0f);

		values_[3].set_x(0.0f);
		values_[3].set_y(0.0f);
		values_[3].set_z(0.0f);
		values_[3].set_w(1.0f);
	}

	void Matrix44::RotationY(float _radiansRot)
	{
		values_[0].set_x(cosf(_radiansRot));
		values_[0].set_y(0.0f);
		values_[0].set_z(-sinf(_radiansRot));
		values_[0].set_w(0.0f);

		values_[1].set_x(0.0f);
		values_[1].set_y(1.0f);
		values_[1].set_z(0.0f);
		values_[1].set_w(0.0f);

		values_[2].set_x(sinf(_radiansRot));
		values_[2].set_y(0.0f);
		values_[2].set_z(cosf(_radiansRot));
		values_[2].set_w(0.0f);

		values_[3].set_x(0.0f);
		values_[3].set_y(0.0f);
		values_[3].set_z(0.0f);
		values_[3].set_w(1.0f);
	}

	void Matrix44::RotationZ(float _radiansRot)
	{
		values_[0].set_x(cosf(_radiansRot));
		values_[0].set_y(sinf(_radiansRot));
		values_[0].set_z(0.0f);
		values_[0].set_w(0.0f);

		values_[1].set_x(-sinf(_radiansRot));
		values_[1].set_y(cosf(_radiansRot));
		values_[1].set_z(0.0f);
		values_[1].set_w(0.0f);

		values_[2].set_x(0.0f);
		values_[2].set_y(0.0f);
		values_[2].set_z(1.0f);
		values_[2].set_w(0.0f);

		values_[3].set_x(0.0f);
		values_[3].set_y(0.0f);
		values_[3].set_z(0.0f);
		values_[3].set_w(1.0f);
	}

	void Matrix44::Rotation(const class Quaternion& quat)
	{
		// this function assumes the quaternion is normalised
		float sqw = quat.w*quat.w;
		float sqx = quat.x*quat.x;
		float sqy = quat.y*quat.y;
		float sqz = quat.z*quat.z;

		values_[0].set_x(( sqx - sqy - sqz + sqw));
		values_[1].set_y((-sqx + sqy - sqz + sqw));
		values_[2].set_z((-sqx - sqy + sqz + sqw));
    
		float tmp1 = quat.x*quat.y;
		float tmp2 = quat.z*quat.w;
		values_[0].set_y(2.0f * (tmp1 + tmp2));
		values_[1].set_x(2.0f * (tmp1 - tmp2));
    
		tmp1 = quat.x*quat.z;
		tmp2 = quat.y*quat.w;
		values_[0].set_z(2.0f * (tmp1 - tmp2));
		values_[2].set_x(2.0f * (tmp1 + tmp2));
		tmp1 = quat.y*quat.z;
		tmp2 = quat.x*quat.w;
		values_[1].set_z(2.0f * (tmp1 + tmp2));
		values_[2].set_y(2.0f * (tmp1 - tmp2));

		values_[0].set_w(0.0f);
		values_[1].set_w(0.0f);
		values_[2].set_w(0.0f);

		values_[3].set_x(0.0f);
		values_[3].set_y(0.0f);
		values_[3].set_z(0.0f);
		values_[3].set_w(1.0f);
	}

	void Matrix44::Scale(const Vector4& scale)
	{
		SetIdentity();
		values_[0].set_x(scale.x());
		values_[1].set_y(scale.y());
		values_[2].set_z(scale.z());
	}

	Vector4 Matrix44::GetScale() const
	{
		Vector4 scale_x(values_[0].x(), values_[0].y(), values_[0].z());
		Vector4 scale_y(values_[1].x(), values_[1].y(), values_[1].z());
		Vector4 scale_z(values_[2].x(), values_[2].y(), values_[2].z());

		return Vector4(scale_x.Length(), scale_y.Length(), scale_z.Length());
	}

	void Matrix44::SetTranslation(const Vector4& _trans)
	{
		values_[3].set_x(_trans.x());
		values_[3].set_y(_trans.y());
		values_[3].set_z(_trans.z());
	}


	const Vector4 Matrix44::GetTranslation() const
	{
		return Vector4(values_[3].x(), values_[3].y(), values_[3].z());
	}

	void Matrix44::Transpose(const Matrix44& matrix)
	{
		for (Int32 rowNum = 0; rowNum < 4; ++rowNum)
			values_[rowNum] = matrix.GetColumn(rowNum);
	}

	void Matrix44::AffineInverse(const Matrix44& matrix)
	{
		Transpose(matrix);
		values_[3].set_x(0.0f);
		values_[3].set_y(0.0f);
		values_[3].set_z(0.0f);
		values_[0].set_w(0.0f);
		values_[1].set_w(0.0f);
		values_[2].set_w(0.0f);

		Vector4 pos, invTrans;

		pos = matrix.GetTranslation();
		pos = pos * -1.0f;

		invTrans = pos.Transform(*this);

		SetTranslation(invTrans);
	}

	void Matrix44::NormaliseRotation()
	{
		gef::Quaternion rotation;
		rotation.SetFromMatrix(*this);
		gef::Vector4 translation = GetTranslation();

		rotation.Normalise();
		Rotation(rotation);
		SetTranslation(translation);
	}

	float Matrix44::CalculateDeterminant() const
	{
		float t[3], v[4];

		t[0] = values_[2].z() * values_[3].w() - values_[2].w() * values_[3].z();
		t[1] = values_[1].z() * values_[3].w() - values_[1].w() * values_[3].z();
		t[2] = values_[1].z() * values_[2].w() - values_[1].w() * values_[2].z();
		v[0] = values_[1].y() * t[0] - values_[2].y() * t[1] + values_[3].y() * t[2];
		v[1] = -values_[1].x() * t[0] + values_[2].x() * t[1] - values_[3].x() * t[2];

		t[0] = values_[1].x() * values_[2].y() - values_[2].x() * values_[1].y();
		t[1] = values_[1].x() * values_[3].y() - values_[3].x() * values_[1].y();
		t[2] = values_[2].x() * values_[3].y() - values_[3].x() * values_[2].y();
		v[2] = values_[3].w() * t[0] - values_[2].w() * t[1] + values_[1].w() * t[2];
		v[3] = -values_[3].z() * t[0] + values_[2].z() * t[1] - values_[1].z() * t[2];

		return values_[0].x() * v[0] + values_[0].y() * v[1] + values_[0].z() * v[2] + values_[0].w() * v[3];
	}

	void Matrix44::Inverse(const Matrix44 matrix, float* determinant)
	{
		int a, i, j;
		Vector4 v, vec[3];
		float det;

		det = matrix.CalculateDeterminant();
		if ( det!= 0.0f )
		{
			for (i=0; i<4; i++)
			{
				for (j=0; j<4; j++)
				{
					if (j != i )
					{
						a = j;
						if ( j > i )
							a = a-1;

						vec[a] = matrix.GetRow(j);
					}
				}

				v = vec[0].CrossProduct3(vec[1], vec[2]);


				float temp = powf(-1.0f, (float)i) / det;
				SetColumn(i, Vector4(temp*v.x(), temp*v.y(), temp*v.z(), temp*v.w()));
			}
	   }

		if(determinant)
			*determinant = det;
	}
}