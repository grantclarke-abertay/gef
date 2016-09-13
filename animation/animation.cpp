#include <animation/animation.h>

namespace gef
{
	AnimNode::AnimNode(Type type) :
		type_(type),
		name_id_(0)
	{
	}

	AnimNode::~AnimNode()
	{
	}

	float AnimNode::GetMaximumKeyTime() const
	{
		return 0.0f;
	}

	bool AnimNode::Read(std::istream& stream)
	{
		stream.read((char*)&name_id_, sizeof(StringId));
		stream.read((char*)&type_, sizeof(Type));

		return true;
	}

	bool AnimNode::Write(std::ostream& stream) const
	{
		stream.write((char*)&name_id_, sizeof(StringId));
		stream.write((char*)&type_, sizeof(Type));

		return true;
	}



	TransformAnimNode::TransformAnimNode() :
		AnimNode(AnimNode::kTransform)
	{
	}

	TransformAnimNode::~TransformAnimNode()
	{
	}

	const Vector4 TransformAnimNode::GetTranslation(const float _time) const
	{
		return GetVector(_time, this->translation_keys_);
	}

	const Vector4 TransformAnimNode::GetScale(const float _time) const
	{
		return GetVector(_time, this->scale_keys_);
	}

	const Quaternion TransformAnimNode::GetRotation(const float _time) const
	{
		Quaternion result;
		result.Identity();

		const QuaternionKey* pPrevKey = NULL;
		const QuaternionKey* pNextKey = NULL;
		UInt32 keyIndex;
		for(keyIndex=0;keyIndex<this->rotation_keys_.size();keyIndex++)
		{

			if(this->rotation_keys_[keyIndex].time > _time)
			{
				pNextKey = &this->rotation_keys_[keyIndex];

				if(keyIndex > 0)
					pPrevKey = &this->rotation_keys_[keyIndex-1];

				break;
			}
		}

		if(keyIndex == this->rotation_keys_.size())
		{
			pNextKey =  &this->rotation_keys_[keyIndex-1];
		}

		if(pPrevKey)
		{
			if(pNextKey)
			{
				float t = (_time - pPrevKey->time) / (pNextKey->time - pPrevKey->time);
				result.Slerp(pPrevKey->value, pNextKey->value, t);
			}
			else
				result = pPrevKey->value;
		}
		else if(pNextKey)
				result = pNextKey->value;

		return result;
	}

	const Vector4 TransformAnimNode::GetVector(float _time, const std::vector<Vector3Key>& _keys) const
	{
		Vector4 result(0.f, 0.f, 0.f);

		const Vector3Key* pPrevKey = NULL;
		const Vector3Key* pNextKey = NULL;
		UInt32 keyIndex;
		for(keyIndex=0;keyIndex<_keys.size();keyIndex++)
		{

			if(_keys[keyIndex].time > _time)
			{
				pNextKey = &_keys[keyIndex];

				if(keyIndex > 0)
					pPrevKey = &_keys[keyIndex-1];

				break;
			}
		}

		if(keyIndex == _keys.size())
		{
			pNextKey =  &_keys[keyIndex-1];
		}

		if(pPrevKey)
		{
			if(pNextKey)
			{
				float t = (_time - pPrevKey->time) / (pNextKey->time - pPrevKey->time);
				result.Lerp(pPrevKey->value, pNextKey->value, t);
			}
			else
				result = pPrevKey->value;
		}
		else if(pNextKey)
			result = pNextKey->value;

		return result;
	}

	float TransformAnimNode::GetMaximumKeyTime() const
	{
		float maximum_key_time = 0.0f;

		if(scale_keys().size() > 0)
		{
			float key_time = scale_keys().back().time;
			if(key_time > maximum_key_time)
				maximum_key_time = key_time;
		}

		if(rotation_keys().size() > 0)
		{
			float key_time = rotation_keys().back().time;
			if(key_time > maximum_key_time)
				maximum_key_time = key_time;
		}

		if(translation_keys().size() > 0)
		{
			float key_time = translation_keys().back().time;
			if(key_time > maximum_key_time)
				maximum_key_time = key_time;
		}


		return maximum_key_time;
	}

	bool TransformAnimNode::Read(std::istream& stream)
	{
		// name_id and type have already been read so don't read them in here

		// scale
		Int32 num_scale_keys;
		stream.read((char*)&num_scale_keys, sizeof(Int32));
		if(num_scale_keys > 0)
		{
			scale_keys_.resize(num_scale_keys);
			stream.read((char*)&scale_keys_.front(), sizeof(Vector3Key)*num_scale_keys);
		}
		// rotate
		Int32 num_rotation_keys;
		stream.read((char*)&num_rotation_keys, sizeof(Int32));
		if(num_rotation_keys > 0)
		{
			rotation_keys_.resize(num_rotation_keys);
			stream.read((char*)&rotation_keys_.front(), sizeof(QuaternionKey)*num_rotation_keys);
		}
		// translate
		Int32 num_translation_keys;
		stream.read((char*)&num_translation_keys, sizeof(Int32));
		if(num_translation_keys > 0)
		{
			translation_keys_.resize(num_translation_keys);
			stream.read((char*)&translation_keys_.front(), sizeof(Vector3Key)*num_translation_keys);
		}
		return true;
	}

	bool TransformAnimNode::Write(std::ostream& stream) const
	{
		bool success = AnimNode::Write(stream);

		Int32 num_scale_keys = (Int32)scale_keys_.size();
		stream.write((char*)&num_scale_keys, sizeof(Int32));
		if(num_scale_keys > 0)
			stream.write((char*)&scale_keys_.front(), sizeof(Vector3Key)*num_scale_keys);
		Int32 num_rotation_keys = (Int32)rotation_keys_.size();
		stream.write((char*)&num_rotation_keys, sizeof(Int32));
		if(num_rotation_keys > 0)
			stream.write((char*)&rotation_keys_.front(), sizeof(QuaternionKey)*num_rotation_keys);
		Int32 num_translation_keys = (Int32)translation_keys_.size();
		stream.write((char*)&num_translation_keys, sizeof(Int32));
		if(num_translation_keys > 0)
			stream.write((char*)&translation_keys_.front(), sizeof(Vector3Key)*num_translation_keys);

		return success;
	}


	ChannelAnimNode::ChannelAnimNode() :
		AnimNode(AnimNode::kChannel)
	{
	}

	ChannelAnimNode::~ChannelAnimNode()
	{
	}

	float ChannelAnimNode::GetMaximumKeyTime() const
	{
		float maximum_key_time = 0.0f;

		if(keys_.size() > 0)
		{
			float key_time = keys_.back().time;
			if(key_time > maximum_key_time)
				maximum_key_time = key_time;
		}
		return maximum_key_time;
	}

	float ChannelAnimNode::GetValue(const float time) const
	{
		float result = 0.0f;

		const ChannelKey* pPrevKey = NULL;
		const ChannelKey* pNextKey = NULL;
		UInt32 keyIndex;
		for(keyIndex=0;keyIndex<keys_.size();keyIndex++)
		{

			if(keys_[keyIndex].time > time)
			{
				pNextKey = &keys_[keyIndex];

				if(keyIndex > 0)
					pPrevKey = &keys_[keyIndex-1];

				break;
			}
		}

		if(keyIndex == keys_.size())
		{
			pNextKey =  &keys_[keyIndex-1];
		}

		if(pPrevKey)
		{
			if(pNextKey)
			{
				float t = (time - pPrevKey->time) / (pNextKey->time - pPrevKey->time);
				result = (1.0f - t)*pPrevKey->value +t*pNextKey->value;
			}
			else
				result = pPrevKey->value;
		}
		else if(pNextKey)
			result = pNextKey->value;

		return result;
	}

	bool ChannelAnimNode::Read(std::istream& stream)
	{
		// name_id and type have already been read so don't read them in here

		Int32 num_keys;
		stream.read((char*)&num_keys, sizeof(Int32));
		if(num_keys > 0)
		{
			keys_.resize(num_keys);
			stream.read((char*)&keys_.front(), sizeof(ChannelKey)*num_keys);
		}
		return true;
	}

	bool ChannelAnimNode::Write(std::ostream& stream) const
	{
		bool success = AnimNode::Write(stream);

		Int32 num_keys = (Int32)keys_.size();
		stream.write((char*)&num_keys, sizeof(Int32));
		if(num_keys > 0)
			stream.write((char*)&keys_.front(), sizeof(ChannelKey)*num_keys);

		return success;
	}

	Animation::Animation():
		duration_(0.0f),
		start_time_(0.0f),
		end_time_(0.0f),
		name_id_(0)
	{
	}

	Animation::~Animation()
	{
		for(std::map<StringId, AnimNode*>::iterator anim_node_iter=anim_nodes_.begin(); anim_node_iter != anim_nodes_.end(); ++anim_node_iter)
			delete anim_node_iter->second;
	}

	Animation::Animation(const class Animation& animation)
	{
		duration_ = animation.duration();
		start_time_ = animation.start_time();
		end_time_ = animation.end_time();
		name_id_ = animation.name_id();
		for (std::map<StringId, AnimNode*>::const_iterator anim_node_iter = animation.anim_nodes().begin(); anim_node_iter != animation.anim_nodes().end(); ++anim_node_iter)
		{
			AnimNode* anim_node = NULL;
			switch (anim_node_iter->second->type())
			{
				case AnimNode::kTransform:
					anim_node = new TransformAnimNode(*(static_cast<TransformAnimNode*>(anim_node_iter->second)));
				break;

				case AnimNode::kChannel:
					anim_node = new ChannelAnimNode(*(static_cast<ChannelAnimNode*>(anim_node_iter->second)));
				break;
			}

			anim_nodes_[anim_node_iter->first] = anim_node;
		}
	}


	void Animation::AddNode(AnimNode* node)
	{
		if(node)
		{
			std::map<StringId, AnimNode*>::iterator anim_node_iter=anim_nodes_.find(node->name_id());
			if(anim_node_iter == anim_nodes_.end())
				anim_nodes_[node->name_id()] = node;
		}
	}


	const AnimNode* Animation::FindNode(const StringId name) const
	{
		const AnimNode* result = NULL;
		std::map<StringId, AnimNode*>::const_iterator anim_node_iter=anim_nodes_.find(name);
		if(anim_node_iter != anim_nodes_.end())
			result = anim_node_iter->second;

		return result;
	}

	void Animation::CalculateDuration()
	{
		// if start and end time haven't been set then duration is calculated from key times
		if(start_time_ == 0.0f && end_time_ == 0.0f)
		{
			duration_ = 0.0f;
			for(std::map<StringId, AnimNode*>::iterator anim_node_iter=anim_nodes_.begin(); anim_node_iter != anim_nodes_.end(); ++anim_node_iter)
			{
				float maximum_key_time = anim_node_iter->second->GetMaximumKeyTime();
				if(maximum_key_time > duration_)
					duration_ = maximum_key_time;
			}

			set_start_time(0.0f);
			set_end_time(duration_);
		}
		else
		{
			duration_ = end_time_ - start_time_;
		}
	}

	bool Animation::Read(std::istream& stream)
	{
		stream.read((char*)&name_id_, sizeof(StringId));
		stream.read((char*)&start_time_, sizeof(float));
		stream.read((char*)&end_time_, sizeof(float));

		Int32 num_anim_nodes;
		stream.read((char*)&num_anim_nodes, sizeof(Int32));
		bool success = true;

		for(Int32 anim_node_num=0; anim_node_num < num_anim_nodes; ++anim_node_num)
		{
			StringId name_id;
			AnimNode::Type type;

			stream.read((char*)&name_id, sizeof(StringId));
			stream.read((char*)&type, sizeof(AnimNode::Type));

			AnimNode* anim_node = NULL;
			switch(type)
			{
			case AnimNode::kTransform:
				anim_node = new TransformAnimNode();
				break;

			case AnimNode::kChannel:
				anim_node = new ChannelAnimNode();
				break;
			}

			anim_node->set_name_id(name_id);
			success = anim_node->Read(stream);
			if(!success)
				break;

			AddNode(anim_node);
		}
		
		if(success)
			CalculateDuration();

		return success;
	}

	bool Animation::Write(std::ostream& stream) const
	{
		stream.write((char*)&name_id_, sizeof(StringId));
		stream.write((char*)&start_time_, sizeof(float));
		stream.write((char*)&end_time_, sizeof(float));

		Int32 num_anim_nodes = (Int32)anim_nodes_.size();
		stream.write((char*)&num_anim_nodes, sizeof(Int32));
		for(std::map<StringId, AnimNode*>::const_iterator anim_node_iter=anim_nodes_.begin(); anim_node_iter != anim_nodes_.end(); ++anim_node_iter)
		{
			anim_node_iter->second->Write(stream);
		}

		return true;
	}
}
