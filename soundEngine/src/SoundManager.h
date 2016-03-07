#ifndef ENGINE_SOUNDMANAGER_H
#define ENGINE_SOUNDMANAGER_H

#include <vector>
#include "UrchinCommon.h"

#include "AudioController.h"
#include "device/DeviceManager.h"
#include "sound/Sound.h"
#include "sound/spatial/PointSound.h"
#include "trigger/SoundTrigger.h"
#include "behavior/SoundBehavior.h"

namespace urchin
{

	class SoundManager
	{
		public:
			SoundManager();
			~SoundManager();

			void addSound(Sound *, SoundTrigger *);
			void removeSound(const Sound *);
			void changeSoundTrigger(const Sound *, SoundTrigger *);

			std::vector<const SoundTrigger *> getSoundTriggers() const;
			SoundTrigger *retrieveSoundTriggerFor(const Sound *) const;

			void process(const Point3<float> &);
			void process();

		private:
			void deleteAudioController(AudioController *);

			DeviceManager deviceManager;

			std::vector<AudioController *> audioControllers;
	};

}

#endif
