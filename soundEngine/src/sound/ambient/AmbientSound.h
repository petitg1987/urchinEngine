#ifndef ENGINE_AMBIENTSOUND_H
#define ENGINE_AMBIENTSOUND_H

#include <string>

#include "sound/Sound.h"

namespace urchin
{

	/**
	 * Ambient sound (unattenuated)
	 */
	class AmbientSound : public Sound
	{
		public:
			AmbientSound(const std::string &);
			~AmbientSound();

			Sound::SoundType getSoundType() const;
	};

}

#endif
