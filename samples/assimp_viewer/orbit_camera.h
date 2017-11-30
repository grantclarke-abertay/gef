#ifndef _ORBIT_CAMERA_H
#define _ORBIT_CAMERA_H

#include <maths/vector4.h>
#include <maths/vector2.h>
#include <maths/matrix44.h>

class OrbitCamera
{
public:

	enum Movement
	{
		kNone,
		kRotate,
		kPan,
		kZoom
	};
	OrbitCamera();

	void Update(float ticks, const gef::Vector4& mouse_rel, const Movement movement);

	inline void set_eye(const gef::Vector4& value) { eye_ = value; }
	inline void set_lookat(const gef::Vector4& value) { lookat_ = value; }
	inline void set_up(const gef::Vector4& value) { up_ = value; }
	inline void set_rotate_speed(const float value) { rotate_speed_ = value; }
	inline void set_movement_speed(const float value) { movement_speed_ = value; }
	inline void set_min_zoom_distance(const float value) { min_zoom_distance_ = value; }

	inline const gef::Vector4& eye() const { return eye_; }
	inline const gef::Vector4& lookat() const { return lookat_; }
	inline const gef::Vector4& up() const { return up_; }
protected:
	gef::Vector4 eye_;
	gef::Vector4 lookat_;
	gef::Vector4 up_;

	gef::Vector2 last_mouse_position_;
	bool last_mouse_position_set_;
	float rotate_speed_;
	float pitch_speed_;
	float zoom_distance_;
	float min_zoom_distance_;
	float movement_speed_;
};

#endif // _ORBIT_CAMERA_H