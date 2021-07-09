#pragma once

#include <string>
#include <iostream>

#include <player/AudioPlayer.h>
#include <player/stream/StreamUpdateWorker.h>
#include <sound/Sound.h>

namespace urchin {

    class AudioStreamPlayer : public AudioPlayer {
        public:
            AudioStreamPlayer(const Sound*, StreamUpdateWorker&);

            void play() override;
            void playLoop() override;
            bool isPlaying() override;

            void pause() override;
            bool isPaused() override;

            void stop() override;

        private:
            void play(bool);

            StreamUpdateWorker& streamUpdateWorker;
    };

}
