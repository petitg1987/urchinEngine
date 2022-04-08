#pragma once

#include <AL/al.h>
#include <memory>

#include <player/stream/AudioStreamPlayer.h>
#include <player/filereader/SoundFileReader.h>
#include <player/stream/StreamChunk.h>

namespace urchin {

    /**
    * Task for the stream worker
    */
    class StreamUpdateTask {
        public:
            StreamUpdateTask(const AudioStreamPlayer&, unsigned int, bool);

            ALuint getSourceId() const;
            bool isSourceStopped() const;
            const SoundFileReader& getSoundFileReader() const;
            bool isPlayLoop() const;

            std::string getSoundFilename() const;

            StreamChunk& getStreamChunk(unsigned int);
            void setInitialReadSamples(unsigned int);
            void initializeReadCursor();

        private:
            const AudioStreamPlayer& audioStreamPlayer;
            SoundFileReader soundFileReader;
            bool playLoop;

            std::vector<StreamChunk> streamChunks;
            unsigned int initialReadSamples;
    };

}
