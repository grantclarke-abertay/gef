#include "motion_clip_player.h"
#include <animation/animation.h>
#include <system/debug_log.h>

MotionClipPlayer::MotionClipPlayer() :
clip_(NULL),
anim_time_(0.0f),
playback_speed_(1.0f),
looping_(false)
{
}

bool MotionClipPlayer::Update(const float delta_time, const gef::SkeletonPose& bind_pose)
{
	bool finished = false;

	if (clip_)
	{
		// update the animation playback time
		anim_time_ += delta_time*playback_speed_;

		// check to see if the playback has reached the end of the animation
		if(anim_time_ > clip_->duration())
		{
			// if the animation is looping then wrap the playback time round to the beginning of the animation
			// other wise set the playback time to the end of the animation and flag that we have reached the end
			if(looping_)
				anim_time_ = std::fmodf(anim_time_, clip_->duration());
			else
			{
				anim_time_ = clip_->duration();
				finished = true;
			}
		}

		// add the clip start time to the playback time to calculate the final time
		// that will be used to sample the animation data
		float time = anim_time_+clip_->start_time();

		// sample the animation data at the calculated time
		// any bones that don't have animation data are set to the bind pose
		pose_.SetPoseFromAnim(*clip_, bind_pose, time);
	}
	else
	{
		// no animation associated with this player
		// just set the pose to the bind pose
		pose_ = bind_pose;
	}

	// return true if we have reached the end of the animation, always false when playback is looped
	return finished;
}