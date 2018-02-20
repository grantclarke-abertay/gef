#include "orbit_camera.h"
#include <maths/matrix44.h>
#include <cmath>

OrbitCamera::OrbitCamera()
	: last_mouse_position_set_(false)
	, eye_(0.0f, 0.0f, 1.0f)
	, lookat_(0.0f, 0.0f, 0.0f)
	, up_(0.0f, 1.0f, 0.0f)
	, rotate_speed_(0.1f)
	, min_zoom_distance_(1.0f)
	, movement_speed_(0.5f)
{

}


void OrbitCamera::Update(float ticks, const gef::Vector4& mouse_rel, const Movement movement)
{
	gef::Vector4 displacement = gef::Vector4::kZero;
	gef::Vector4 direction = lookat_ - eye_;

	zoom_distance_ = direction.Length();
	direction.Normalise();

	// current view matrix
	gef::Matrix44 view_matrix;
	view_matrix.LookAt(eye_, lookat_, up_);


	// camera world matrix
	gef::Matrix44 world_matrix;
	world_matrix.AffineInverse(view_matrix);

	float movement_speed_mulitplier = mouse_rel.z();

	if (movement == kZoom)
		movement_speed_mulitplier += mouse_rel.x() + mouse_rel.y();

    // zoom and pan speed is dependant on zoom distance
	float zoom_distance_speed_modifier = (zoom_distance_/min_zoom_distance_);

	zoom_distance_ -= movement_speed_mulitplier*ticks*movement_speed_*zoom_distance_speed_modifier;
	if (zoom_distance_ < min_zoom_distance_)
	{
		displacement += direction*(min_zoom_distance_ - zoom_distance_);
		zoom_distance_ = min_zoom_distance_;
	}

	if (movement == kRotate)
	{
		float pitch = -mouse_rel.y()*ticks*rotate_speed_;
		float yaw = -mouse_rel.x()*ticks*rotate_speed_;

		gef::Matrix44 yMat;
		yMat.RotationY(yaw);

		gef::Matrix44 xMat;
		xMat.RotationX(pitch);

		gef::Matrix44 rotMat = xMat * yMat * world_matrix;

		direction = gef::Vector4(0.0, 0.0f, -1.0f);
		direction = direction.TransformNoTranslation(rotMat);
	}

	gef::Vector4 right = up_.CrossProduct(direction);
	up_ = direction.CrossProduct(right);

	if (movement == kPan)
		displacement += (right*mouse_rel.x() + up_*mouse_rel.y())*ticks*rotate_speed_*zoom_distance_speed_modifier;
//	displacement += (right*mouse_rel.x() + up_*mouse_rel.y())*ticks*rotate_speed_*zoom_distance_speed_modifier;

	lookat_ += displacement;
	eye_ = lookat_ - direction*zoom_distance_;
	right = up_.CrossProduct(direction);
	up_ = direction.CrossProduct(right);
}