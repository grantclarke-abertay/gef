#ifndef _GEF_AUDIO_MANAGER_H
#define _GEF_AUDIO_MANAGER_H

#include <gef.h>

namespace gef
{
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

		static AudioManager* Create();
	protected:
		AudioManager();
	};
}

#endif // _GEF_AUDIO_MANAGER_H
