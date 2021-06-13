#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <player/AudioPlayer.h>
#include <player/stream/StreamUpdateWorker.h>

namespace urchin {

    /**
    * Allow to make the link between sound, sound trigger and the player
    */
    class AudioController {
        public:
            AudioController(Sound*, SoundTrigger*, StreamUpdateWorker*);
            ~AudioController();

            const Sound* getSound() const;
            SoundTrigger* getSoundTrigger() const;
            void changeSoundTrigger(SoundTrigger*);

            void pause();
            void unpause();

            void process(const Point3<float>&);

        private:
            void processTriggerValue(SoundTrigger::TriggerResultValue);

            Sound* sound;
            SoundTrigger* soundTrigger;

            SoundTrigger::TriggerResultValue triggerValue;

            AudioPlayer* audioPlayer;
            bool isPaused;
    };

}
