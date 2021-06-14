#pragma once

#include <string>
#include <AL/al.h>

namespace urchin {

    class Sound {
        public:
            enum SoundType {
                SPATIAL,
                GLOBAL
            };

            explicit Sound(std::string);
            virtual ~Sound();

            virtual SoundType getSoundType() const = 0;
            ALuint getSourceId() const;
            const std::string& getFilename() const;

            bool isStopped() const;
            bool isPaused() const;
            bool isPlaying() const;

            void setInitialVolume(float);
            float getInitialVolume() const;
            void changeVolume(float);

        private:
            void applyVolume() const;

            ALuint sourceId;

            std::string filename;
            float initialVolume;
            float volumePercentageChange;
    };

}
