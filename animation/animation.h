#ifndef _GEF_ANIM_H
#define _GEF_ANIM_H

#include <system/string_id.h>
#include <maths/vector4.h>
#include <maths/quaternion.h>
#include <vector>
#include <map>
#include <istream>

namespace gef
{
	class AnimNode
	{
	public:
		enum Type
		{
			kTransform = 0,
			kChannel
		};

		AnimNode(Type type);
		virtual ~AnimNode();

		virtual float GetMaximumKeyTime() const = 0;

		virtual bool Read(std::istream& stream);
		virtual bool Write(std::ostream& stream) const;

		inline void set_name_id(StringId name_id) { name_id_  = name_id; }
		inline StringId name_id() const { return name_id_; }
		inline Type type() const { return type_; }


	protected:
		Type type_;
	private:
		StringId name_id_;
	};

	struct Vector3Key
	{
		Vector4 value;
		float time;
	};

	struct QuaternionKey
	{
		Quaternion value;
		float time;
	};

	struct ChannelKey
	{
		float value;
		float time;
	};

	class TransformAnimNode : public AnimNode
	{
	public:
		TransformAnimNode();
		~TransformAnimNode();

		const Vector4 GetTranslation(const float time) const;
		const Vector4 GetScale(const float time) const;
		const Quaternion GetRotation(const float time) const;

		inline const std::vector<Vector3Key>& scale_keys() const {return scale_keys_;}
		inline std::vector<Vector3Key>& scale_keys() { return const_cast<std::vector<Vector3Key>&>(static_cast<const TransformAnimNode&>(*this).scale_keys()); }
		inline const std::vector<QuaternionKey>& rotation_keys() const {return rotation_keys_;}
		inline std::vector<QuaternionKey>& rotation_keys() { return const_cast<std::vector<QuaternionKey>&>(static_cast<const TransformAnimNode&>(*this).rotation_keys()); }
		inline const std::vector<Vector3Key>& translation_keys() const {return translation_keys_;}
		inline std::vector<Vector3Key>& translation_keys() { return const_cast<std::vector<Vector3Key>&>(static_cast<const TransformAnimNode&>(*this).translation_keys()); }

		float GetMaximumKeyTime() const;

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

	private:
		const Vector4 GetVector(const float _time, const std::vector<Vector3Key>& keys) const;

		std::vector<Vector3Key> scale_keys_;
		std::vector<QuaternionKey> rotation_keys_;
		std::vector<Vector3Key> translation_keys_;
	};

	class ChannelAnimNode : public AnimNode
	{
	public:
		ChannelAnimNode();
		~ChannelAnimNode();

		float GetValue(const float time) const;

		inline const std::vector<ChannelKey>& keys() const {return keys_;}
		inline std::vector<ChannelKey>& keys() { return const_cast<std::vector<ChannelKey>&>(static_cast<const ChannelAnimNode&>(*this).keys()); }

		float GetMaximumKeyTime() const;

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

	private:
		std::vector<ChannelKey> keys_;
	};

	class Animation
	{
	public:
		Animation();
		~Animation();
		Animation(const class Animation& animation);
		void AddNode(AnimNode* node);
		const AnimNode* FindNode(const StringId name) const;
		void CalculateDuration();

		bool Read(std::istream& stream);
		bool Write(std::ostream& stream) const;

		inline const std::map<StringId, AnimNode*>& anim_nodes() const { return anim_nodes_; }
		inline float duration() const { return duration_; }
		inline void set_start_time(const float start_time) { start_time_ = start_time; }
		inline void set_end_time(const float end_time) { end_time_ = end_time; }
		inline float start_time() const { return start_time_; }
		inline float end_time() const { return end_time_; }

		inline void set_name_id(const StringId name_id) { name_id_ = name_id; }
		inline StringId name_id() const { return name_id_; }
	protected:
		std::map<StringId, AnimNode*> anim_nodes_;
		float duration_;
		float start_time_;
		float end_time_;
		StringId name_id_;

	};
}
#endif // _GEF_ANIM_H
