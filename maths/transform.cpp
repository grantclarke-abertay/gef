#include "transform.h"

namespace gef
{
	Transform::Transform()
	{
	}

	Transform::Transform(const Matrix44& matrix)
	{
		Set(matrix);
	}

	const Matrix44 Transform::GetMatrix() const
	{
		Matrix44 result, scale_matrix, rotation_matrix;
	
		scale_matrix.Scale(scale_);
		rotation_matrix.Rotation(rotation_);
		result = scale_matrix * rotation_matrix;
		result.SetTranslation(translation_);

		return result;
	}

	void Transform::Set(const Matrix44& matrix)
	{
		translation_ = matrix.GetTranslation();
		Quaternion rotation;
		rotation_.SetFromMatrix(matrix);
		rotation_.Normalise();
		scale_ = matrix.GetScale();
	}

	void Transform::Linear2TransformBlend(const gef::Transform& start, const gef::Transform& end, const float time)
	{
		Vector4 scale(1.0f, 1.0f, 1.0f), translation;
		Quaternion rotation;
		scale.Lerp(start.scale(), end.scale(), time);
		translation.Lerp(start.translation(), end.translation(), time);
		rotation.Slerp(start.rotation(), end.rotation(), time);
		set_scale(scale);
		set_rotation(rotation);
		set_translation(translation);
	}

	void Transform::Inverse(const Transform& transform)
	{
		rotation_.Conjugate(transform.rotation());

		Vector4 inv_translation = -transform.translation();

		inv_translation = Quaternion::Rotate(rotation_, inv_translation);
		translation_ = inv_translation;
	}

}