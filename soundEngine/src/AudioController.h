#pragma once

#include <vector>
#include <UrchinCommon.h>

#include <sound/Sound.h>
#include <trigger/SoundTrigger.h>
#include <player/AudioPlayer.h>
#include <player/stream/StreamUpdateWorker.h>

namespace urchin {

    /**
    * Allow to make the link between sound, sound trigger and the players
    */
    class AudioController {
        public:
            AudioController(std::shared_ptr<Sound>, std::shared_ptr<SoundTrigger>, StreamUpdateWorker&);
            ~AudioController();

            Sound& getSound() const;
            SoundTrigger& getSoundTrigger() const;
            void changeSoundTrigger(std::shared_ptr<SoundTrigger>);

            void pauseAll();
            void unpauseAll();

            void process(const Point3<float>&, const std::map<Sound::SoundCategory, float>&);

        private:
            void processTriggerValue(SoundTrigger::TriggerAction);

            std::shared_ptr<Sound> sound;
            std::shared_ptr<SoundTrigger> soundTrigger;
            StreamUpdateWorker& streamUpdateWorker;

            std::vector<std::unique_ptr<AudioPlayer>> audioPlayers;
    };

}
