#ifndef _MOTION_CLIP_PLAYER_H
#define _MOTION_CLIP_PLAYER_H

#include <animation/skeleton.h>

namespace gef
{

};

class MotionClipPlayer
{
public:
	MotionClipPlayer();

	/// @brief Update the pose by sampling current animation clip
	/// @param[in] delta_time	The amount of time to update the playback time by.
	/// @param[in] bind_pose	The bind pose for the skeleton being animated.
	bool Update(const float delta_time, const gef::SkeletonPose& bind_pose);

	const float anim_time() const { return anim_time_; }
	void set_anim_time(const float anim_time) { anim_time_ = anim_time; }

	const float playback_speed() const { return playback_speed_; }
	void set_playback_speed(const float playback_speed) { playback_speed_ = playback_speed; }

	const bool looping() const { return looping_; }
	void set_looping(const bool looping) { looping_ = looping; }

	const gef::Animation* clip() const { return clip_; }
	void set_clip(const gef::Animation* clip) { clip_ = clip; }

	const gef::SkeletonPose& pose() const { return pose_; }

private:
	/// The pose created by sampling the animation clip
	gef::SkeletonPose pose_;

	/// A pointer to the animation clip to be sampled
	const gef::Animation* clip_;

	/// The current playback time the animation clip is being sampled at
	float anim_time_;

	/// The playback speed scaling factor used to adjust the speed the animation is played at
	float playback_speed_;

	/// The flag indicating whether the playback is to be looped or not
	bool looping_;
};

#endif // _MOTION_CLIP_PLAYER_H