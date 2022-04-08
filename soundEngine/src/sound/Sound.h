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

            explicit Sound(std::string, SoundCategory, float);
            virtual ~Sound() = default;

            virtual void initializeSource(ALuint) const = 0;
            virtual void updateSource(ALuint) = 0;

            virtual SoundType getSoundType() const = 0;
            const std::string& getFilename() const;
            SoundCategory getSoundCategory() const;
            float getInitialVolume() const;

            std::vector<int16_t> getPreLoadedChunk(std::size_t, bool) const;

        private:
            void preLoadChunks();

            std::string filename;
            SoundCategory category;
            float initialVolume;

            std::size_t chunkMaxSize;
            std::vector<std::vector<int16_t>> preLoadedChunks;
    };

}
