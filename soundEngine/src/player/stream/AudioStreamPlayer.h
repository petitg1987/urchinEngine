#pragma once

#include <string>

#include <player/AudioPlayer.h>
#include <sound/Sound.h>

namespace urchin {

    class StreamUpdateWorker;

    class AudioStreamPlayer final : public AudioPlayer {
        public:
            AudioStreamPlayer(const Sound&, StreamUpdateWorker&);
            ~AudioStreamPlayer() override;

            void play() override;
            void playLoop() override;

            void pause() override;
            void unpause() override;

            void stop() override;

        private:
            void play(bool);

            StreamUpdateWorker& streamUpdateWorker;
    };

}
