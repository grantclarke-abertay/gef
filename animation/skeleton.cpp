#include <animation/skeleton.h>
#include <animation/animation.h>

namespace gef
{
	Int32 Skeleton::AddJoint(const Joint& joint)
	{
		joints_.push_back(joint);
		return (Int32)joints_.size() - 1;
	}

	Int32 Skeleton::FindJointIndex(const StringId joint_name_id) const
	{
		Int32 result = -1;

		Int32 index=0;
		for(std::vector<Joint>::const_iterator iter=joints().begin();iter != joints().end();++iter, ++index)
		{
			if(iter->name_id == joint_name_id) 
			{
				result = index;
				break;
			}
		}

		return result;
	}

	const Joint* Skeleton::FindJoint(const StringId joint_name) const
	{
		Int32 joint_index = FindJointIndex(joint_name);
		if(joint_index == -1)
			return NULL;
		else
			return &joints_[joint_index];
	}

	SkeletonPose::SkeletonPose() :
	skeleton_(NULL)
	{
	}

	void SkeletonPose::CalculateGlobalPose(const gef::Matrix44 * const pose_transform)
	{
		if(skeleton_)
		{
			const std::vector<Joint>& joints = skeleton_->joints();
			for(UInt32 jointNum=0; jointNum<joints.size(); jointNum++)
			{
				const Joint& joint = joints[jointNum];
				Matrix44 local_pose_matrix = local_pose()[jointNum].GetMatrix();
				Matrix44 global_pose_matrix;
				if(joint.parent == -1)
				{
					global_pose_matrix = local_pose_matrix;
					if(pose_transform)
						global_pose_matrix = global_pose_matrix * (*pose_transform);
				}
				else
					global_pose_matrix = local_pose_matrix * global_pose()[joint.parent];


				if(global_pose_.size() > jointNum )
					global_pose_[jointNum] = global_pose_matrix;
				else
					global_pose_.push_back(global_pose_matrix);
			}
		}
	}

	void SkeletonPose::CalculateLocalPose(const std::vector<Matrix44>& global_pose_matrices)
	{
		if(skeleton_)
		{
			const std::vector<Joint>& joints = skeleton_->joints();
			for(UInt32 jointNum=0; jointNum<joints.size(); jointNum++)
			{
				const Joint& joint = joints[jointNum];
				Matrix44 local_pose_matrix;
				const Matrix44& global_pose_matrix = global_pose_matrices[jointNum];
				if(joint.parent == -1)
					local_pose_matrix = global_pose_matrix;
				else
				{
					Matrix44 inv_parent_matrix;
					inv_parent_matrix.AffineInverse(global_pose_matrices[joint.parent]);
					local_pose_matrix = global_pose_matrix * inv_parent_matrix;
				}

				local_pose_[jointNum].Set(local_pose_matrix);
			}
		}
	}


	void SkeletonPose::CreateBindPose(const Skeleton* const skeleton)
	{
		if(skeleton)
		{
			for(UInt32 jointNum=0;jointNum<skeleton->joints().size();jointNum++)
			{
				Matrix44 local_matrix;
				Matrix44 global_bind_matrix;

				const Joint& joint = skeleton->joints()[jointNum];
				if(joint.parent == -1)
				{
					local_matrix.AffineInverse(joint.inv_bind_pose);
					global_bind_matrix = local_matrix;
				}
				else
				{
					const Joint& parent_joint = skeleton->joints()[joint.parent];
					Matrix44 invParentMatrix = parent_joint.inv_bind_pose;

					global_bind_matrix.AffineInverse(joint.inv_bind_pose);
					local_matrix = global_bind_matrix*invParentMatrix;
				}

				JointPose jointPose;
				jointPose.Set(local_matrix);

				local_pose().push_back(jointPose);
				global_pose_.push_back(global_bind_matrix);
			}

			skeleton_ = skeleton;
		}
	}

	void SkeletonPose::SetPoseFromAnim(const Animation& anim, const SkeletonPose& bind_pose, float time, const bool updateGlobalPose)
	{
		Int32 joint_index=0;
		for(std::vector<JointPose>::iterator joint_iter = local_pose_.begin(); joint_iter != local_pose_.end(); ++joint_iter, ++joint_index)
		{
			const AnimNode* anim_node = anim.FindNode(skeleton_->joints()[joint_index].name_id);
			JointPose& joint_pose = local_pose_[joint_index];

//			if(joint_index != 1)
//				anim_node = NULL;
			if(anim_node)
			{
				if(anim_node->type() == AnimNode::kTransform) // this should always be true since the find uses the joint transform name
				{
					const TransformAnimNode* transform_node = static_cast<const TransformAnimNode*>(anim_node);

					// scale
					if(transform_node->scale_keys().size() > 0.f)
						joint_pose.set_scale(transform_node->GetScale(time));
					else
						joint_pose.set_scale(bind_pose.local_pose()[joint_index].scale());
					joint_pose.set_scale(gef::Vector4(1.f, 1.f, 1.f));

					// rotation
					if(transform_node->rotation_keys().size() > 0.f)
						joint_pose.set_rotation(transform_node->GetRotation(time));
					else
						joint_pose.set_rotation(bind_pose.local_pose()[joint_index].rotation());

					// translation
					if(transform_node->translation_keys().size() > 0.f)
						joint_pose.set_translation(transform_node->GetTranslation(time));
					else
						joint_pose.set_translation(bind_pose.local_pose()[joint_index].translation());
				}
			}
			else
			{
				joint_pose = bind_pose.local_pose()[joint_index];
			}

			// check to see if there is a pose transform
			// if so use it to transform any root joints
//			if(pose_transform && joint_index == 1)
//				joint_pose.Set(joint_pose.GetMatrix() * *pose_transform);

#ifdef REMOVE_BIND_POSE
			gef::Matrix44 inv_local_joint_orient;
			inv_local_joint_orient.AffineInverse(bind_pose.local_pose()[joint_index].GetMatrix());
			inv_local_joint_orient.SetTranslation(gef::Vector4(0.f, 0.f, 0.f));
			joint_pose.Set(inv_local_joint_orient * joint_pose.GetMatrix());
#endif
		}

		if(updateGlobalPose)
			CalculateGlobalPose();
	}

	void SkeletonPose::Linear2PoseBlend(const SkeletonPose& start_pose, const SkeletonPose& end_pose, const float time)
	{
		// assume _startPose _endPose and "this" pose all have the same number of joints
		std::vector<JointPose>::const_iterator start_pose_iter = start_pose.local_pose().begin();
		std::vector<JointPose>::const_iterator end_pose_iter = end_pose.local_pose().begin();
		std::vector<JointPose>::iterator result_pose_iter = local_pose().begin();


		for(;start_pose_iter != start_pose.local_pose().end();start_pose_iter++, end_pose_iter++, result_pose_iter++)
			result_pose_iter->Linear2TransformBlend(*start_pose_iter, *end_pose_iter, time);

		this->CalculateGlobalPose();

	}

	void SkeletonPose::CleanUp()
	{
		skeleton_ = NULL;
		local_pose_.clear();
		global_pose_.clear();
	}

	gef::Matrix44 SkeletonPose::GetGlobalJointTransformFromAnim(const class Animation* anim, const SkeletonPose& bind_pose, float time, const Int32 joint_index)
	{
		const gef::Skeleton* skeleton = bind_pose.skeleton();

		// calculate the transform for this joint
		const AnimNode* anim_node = NULL;
		
		if (anim)
			anim_node = anim->FindNode(skeleton->joints()[joint_index].name_id);
		JointPose joint_pose;
		if(anim_node)
		{
			if(anim_node->type() == AnimNode::kTransform) // this should always be true since the find uses the joint transform name
			{
				const TransformAnimNode* transform_node = static_cast<const TransformAnimNode*>(anim_node);

				// scale
				if(transform_node->scale_keys().size() > 0.f)
					joint_pose.set_scale(transform_node->GetScale(time));
				else
					joint_pose.set_scale(bind_pose.local_pose()[joint_index].scale());
				joint_pose.set_scale(gef::Vector4(1.f, 1.f, 1.f));

				// rotation
				if(transform_node->rotation_keys().size() > 0.f)
					joint_pose.set_rotation(transform_node->GetRotation(time));
				else
					joint_pose.set_rotation(bind_pose.local_pose()[joint_index].rotation());

				// translation
				if(transform_node->translation_keys().size() > 0.f)
					joint_pose.set_translation(transform_node->GetTranslation(time));
				else
					joint_pose.set_translation(bind_pose.local_pose()[joint_index].translation());
			}
		}
		else
		{
			joint_pose = bind_pose.local_pose()[joint_index];
		}

#ifdef REMOVE_BIND_POSE
		gef::Matrix44 inv_local_joint_orient;
		inv_local_joint_orient.AffineInverse(bind_pose.local_pose()[joint_index].GetMatrix());
		inv_local_joint_orient.SetTranslation(gef::Vector4(0.f, 0.f, 0.f));

		joint_pose.Set(inv_local_joint_orient * joint_pose.GetMatrix());
#endif

		// multiply the joint transform by all the parent joint transforms
		Int32 parent = skeleton->joint(joint_index).parent;
		if(parent == -1)
			return joint_pose.GetMatrix();
		else
			return joint_pose.GetMatrix() * GetGlobalJointTransformFromAnim(anim, bind_pose, time, parent);
	}


	gef::Matrix44 SkeletonPose::GetJointTransformFromAnim(const class Animation& anim, const SkeletonPose& bind_pose, float time, const Int32 joint_index)
	{
		const gef::Skeleton* skeleton = bind_pose.skeleton();

		// calculate the transform for this joint
		const AnimNode* anim_node = anim.FindNode(skeleton->joints()[joint_index].name_id);
		JointPose joint_pose;
		if(anim_node)
		{
			if(anim_node->type() == AnimNode::kTransform) // this should always be true since the find uses the joint transform name
			{
				const TransformAnimNode* transform_node = static_cast<const TransformAnimNode*>(anim_node);

				// scale
				if(transform_node->scale_keys().size() > 0.f)
					joint_pose.set_scale(transform_node->GetScale(time));
				else
					joint_pose.set_scale(bind_pose.local_pose()[joint_index].scale());
				joint_pose.set_scale(gef::Vector4(1.f, 1.f, 1.f));

				// rotation
				if(transform_node->rotation_keys().size() > 0.f)
					joint_pose.set_rotation(transform_node->GetRotation(time));
				else
					joint_pose.set_rotation(bind_pose.local_pose()[joint_index].rotation());

				// translation
				if(transform_node->translation_keys().size() > 0.f)
					joint_pose.set_translation(transform_node->GetTranslation(time));
				else
					joint_pose.set_translation(bind_pose.local_pose()[joint_index].translation());
			}
		}
		else
		{
			joint_pose = bind_pose.local_pose()[joint_index];
		}

		return joint_pose.GetMatrix();
	}



	bool Skeleton::Read(std::istream& stream)
	{
		Int32 num_joints;
		stream.read((char*)&num_joints, sizeof(Int32));
		joints_.resize(num_joints);
		stream.read((char*)&joints_.front(), sizeof(Joint)*num_joints);

		return true;
	}

	bool Skeleton::Write(std::ostream& stream) const
	{
		Int32 num_joints = (Int32)joints_.size();
		stream.write((char*)&num_joints, sizeof(Int32));
		stream.write((char*)&joints_.front(), sizeof(Joint)*num_joints);

		return true;
	}
}
