#pragma once

#include <UrchinCommon.h>

#include "sound/Sound.h"
#include "trigger/SoundTrigger.h"
#include "player/stream/StreamUpdateWorker.h"
#include "SoundComponent.h"

namespace urchin {

    /**
    * Allow to make the link between sound, sound trigger and the players
    */
    class AudioController {
        public:
            AudioController(std::shared_ptr<SoundComponent>, StreamUpdateWorker&);
            ~AudioController();

            SoundComponent& getSoundComponent() const;

            void pauseAll();
            void unpauseAll();
            std::size_t getPlayersCount() const;

            void process(const Point3<float>&, const std::map<Sound::SoundCategory, float>&);

        private:
            void processTriggerValue(SoundTrigger::TriggerAction);

            std::shared_ptr<SoundComponent> soundComponent;
            StreamUpdateWorker& streamUpdateWorker;

            std::list<AudioStreamPlayer> audioStreamPlayers;
    };

}
