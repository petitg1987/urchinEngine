#ifndef URCHINENGINE_SOUNDMANAGER_H
#define URCHINENGINE_SOUNDMANAGER_H

#include <vector>
#include <thread>
#include <UrchinCommon.h>

#include <AudioController.h>
#include <device/DeviceManager.h>
#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <player/stream/StreamUpdateWorker.h>

namespace urchin {

    class SoundManager {
        public:
            SoundManager();
            ~SoundManager();

            void addSound(Sound*, SoundTrigger*);
            void removeSound(const Sound*);
            void changeSoundTrigger(const Sound*, SoundTrigger*);

            std::vector<const SoundTrigger*> getSoundTriggers() const;
            SoundTrigger* retrieveSoundTriggerFor(const Sound*) const;

            void pause();
            void unpause();

            void controlExecution();

            void process(const Point3<float>&);
            void process();

        private:
            static void deleteAudioController(AudioController*);

            std::vector<AudioController*> audioControllers;

            //stream chunk updater thread
            StreamUpdateWorker* streamUpdateWorker;
            std::thread *streamUpdateWorkerThread;
    };

}

#endif
