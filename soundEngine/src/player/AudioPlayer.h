#ifndef ENGINE_AUDIOPLAYER_H
#define ENGINE_AUDIOPLAYER_H

#include "sound/Sound.h"

namespace urchin
{

	class AudioPlayer
	{
		public:
			AudioPlayer(const Sound *);
			virtual ~AudioPlayer();

			virtual void play() = 0;
			virtual void playLoop() = 0;
			virtual void pause() = 0;
			virtual void stop() = 0;

			const Sound *getSound() const;

		private:
			const Sound *sound;
	};

}

#endif
