#pragma once

#include <string>
#include <AL/al.h>

#include <player/filereader/ChunkPreLoader.h>

namespace urchin {

    class Sound {
        public:
            enum SoundType {
                LOCALIZABLE,
                GLOBAL
            };

            enum SoundCategory {
                MUSIC, //ambient music in game or in the menus
                EFFECTS //game effects (wind, explosion, radio playing music...), UI feedbacks
            };

            explicit Sound(std::string, SoundCategory, float);
            virtual ~Sound() = default;

            void preLoadChunks(ChunkPreLoader&);
            bool hasPreLoadedChunks() const;
            std::vector<int16_t> getPreLoadedChunk(std::size_t, bool) const;

            virtual void initializeSource(ALuint) const = 0;
            virtual void updateSource(ALuint) = 0;

            virtual SoundType getSoundType() const = 0;
            const std::string& getFilename() const;
            SoundCategory getSoundCategory() const;
            float getInitialVolume() const;

            virtual std::unique_ptr<Sound> clone() const = 0;

        private:
            std::string filename;
            SoundCategory category;
            float initialVolume;

            std::vector<std::vector<int16_t>> preLoadedChunks;
            std::size_t chunkMaxSize = 0;
    };

}
