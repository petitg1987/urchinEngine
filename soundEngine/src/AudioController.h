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
            AudioController(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>, StreamUpdateWorker&);
            ~AudioController();

            Sound& getSound() const;
            SoundTrigger& getSoundTrigger() const;
            void changeSoundTrigger(std::shared_ptr<SoundTrigger>);
            AudioPlayer& getAudioPlayer() const;

            void pause();
            void unpause();

            void process(const Point3<float>&);

        private:
            void processTriggerValue(SoundTrigger::TriggerResultValue);

            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;

            SoundTrigger::TriggerResultValue triggerValue;

            std::unique_ptr<AudioPlayer> audioPlayer;
            bool isPaused;
    };

}
