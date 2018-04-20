#ifndef _GEF_TRANSFORM_H
#define _GEF_TRANSFORM_H

#include <maths/matrix44.h>
#include <maths/quaternion.h>
#include <maths/vector4.h>

namespace gef
{
	class Transform
	{
	public:
		Transform();
		Transform(const Matrix44& matrix);
		const Matrix44 GetMatrix() const;
		void Set(const Matrix44& matrix);
		void Linear2TransformBlend(const gef::Transform& start, const gef::Transform& end, const float time);
		void Inverse(const Transform& transform);

		inline void set_rotation(const Quaternion& rot) { rotation_ = rot; }
		inline const Quaternion& rotation() const { return rotation_; }
		inline void set_scale(const Vector4& scale) { scale_ = scale; }
		inline const Vector4& scale() const { return scale_; }
		inline void set_translation(const Vector4& trans) { translation_ = trans; }
		inline const Vector4& translation() const { return translation_; }
	private:
		Quaternion rotation_;
		Vector4 translation_;
		Vector4 scale_;
	};
}

#endif // _GEF_TRANSFORM_H
