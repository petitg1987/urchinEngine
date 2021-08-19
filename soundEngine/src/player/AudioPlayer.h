#pragma once

#include <sound/Sound.h>

namespace urchin {

    class AudioPlayer {
        public:
            explicit AudioPlayer(const Sound&);
            virtual ~AudioPlayer();

            virtual void play() = 0;
            virtual void playLoop() = 0;
            bool isPlaying() const;

            virtual void pause() = 0;
            bool isPaused() const;

            virtual void stop() = 0;
            bool isStopped() const;

            void changeVolume(float);

            const Sound& getSound() const;
            ALuint getSourceId() const;

        protected:
            void applyVolume() const;

        private:
            const Sound& sound;

            ALuint sourceId;
            float volumePercentageChange;
    };

}
