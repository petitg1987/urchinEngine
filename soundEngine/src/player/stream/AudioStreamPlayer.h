#pragma once

#include <string>
#include <iostream>

#include <player/AudioPlayer.h>
#include <sound/Sound.h>

namespace urchin {

    class StreamUpdateWorker;

    class AudioStreamPlayer : public AudioPlayer {
        public:
            AudioStreamPlayer(const Sound&, StreamUpdateWorker&);

            void play() override;
            void playLoop() override;

            void pause() override;

            void stop() override;

        private:
            void play(bool);

            StreamUpdateWorker& streamUpdateWorker;
    };

}
