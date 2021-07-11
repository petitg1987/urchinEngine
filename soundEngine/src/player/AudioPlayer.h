#pragma once

#include <sound/Sound.h>

namespace urchin {

    class AudioPlayer {
        public:
            explicit AudioPlayer(const Sound&);
            virtual ~AudioPlayer() = default;

            virtual void play() = 0;
            virtual void playLoop() = 0;
            virtual bool isPlaying() = 0;

            virtual void pause() = 0;
            virtual bool isPaused() = 0;

            virtual void stop() = 0;

            const Sound& getSound() const;

        private:
            const Sound& sound;
    };

}
