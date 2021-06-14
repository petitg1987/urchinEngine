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

            enum SoundCategory {
                MUSIC, //ambient music in game or in the menus
                EFFECTS //game effects (wind, explosion, radio playing music...), UI feedbacks
            };

            explicit Sound(std::string, SoundCategory);
            virtual ~Sound();

            virtual SoundType getSoundType() const = 0;
            ALuint getSourceId() const;
            const std::string& getFilename() const;
            SoundCategory getSoundCategory() const;

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
            SoundCategory category;

            float initialVolume;
            float volumePercentageChange;
    };

}
