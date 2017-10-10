#ifndef _GEF_AUDIO_MANAGER_H
#define _GEF_AUDIO_MANAGER_H

#include <gef.h>

namespace gef
{

	struct VolumeInfo
	{
		float volume;
		float pan;

		VolumeInfo() :
			volume(1.0f),
			pan(0.0f)
		{
		}
	};

	class Platform;

	class AudioManager
	{
	public:
		virtual ~AudioManager();

		virtual Int32 LoadSample(const char *strFileName, const Platform& platform) = 0;
		virtual Int32 LoadMusic(const char *strFileName, const Platform& platform) = 0;
		virtual void UnloadMusic() = 0;
		virtual void UnloadSample(Int32 sample_num) = 0;
		virtual void UnloadAllSamples() = 0;

		virtual Int32 PlayMusic() = 0;
		virtual Int32 StopMusic() = 0;
		virtual Int32 PlaySample(const Int32 sample_index, const bool looping = false) = 0;
		virtual Int32 StopPlayingSampleVoice(const Int32 voice_index) = 0;

		virtual Int32 SetSamplePitch(const Int32 voice_index, float pitch) = 0;
		virtual Int32 SetMusicPitch(float pitch) = 0;
		virtual Int32 GetSampleVoiceVolumeInfo(const Int32 voice_index, struct VolumeInfo& volume_info) = 0;
		virtual Int32 SetSampleVoiceVolumeInfo(const Int32 voice_index, const struct VolumeInfo& volume_info) = 0;
		virtual Int32 GetMusicVolumeInfo(struct VolumeInfo& volume_info) = 0;
		virtual Int32 SetMusicVolumeInfo(const struct VolumeInfo& volume_info) = 0;
		virtual Int32 SetMasterVolume(float volume) = 0;

		virtual bool sample_voice_playing(const UInt32 voice_index) = 0;
		virtual bool sample_voice_looping(const UInt32 voice_index) = 0;


		static AudioManager* Create();
	protected:
		AudioManager();
	};
}

#endif // _GEF_AUDIO_MANAGER_H
