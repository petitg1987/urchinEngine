#pragma once

#include <AL/al.h>
#include <memory>

#include <sound/Sound.h>
#include <player/filereader/SoundFileReader.h>
#include <player/stream/StreamChunk.h>

namespace urchin {

    /**
    * Task for the stream worker
    */
    class StreamUpdateTask {
        public:
            StreamUpdateTask(const Sound*, StreamChunk*, bool);

            ALuint getSourceId() const;
            bool isSourceStopped() const;
            const SoundFileReader& getSoundFileReader() const;
            bool isPlayLoop() const;

            std::string getSoundFilename() const;

            StreamChunk& getStreamChunk(unsigned int);
            StreamChunk* getStreamChunks();

        private:
            const Sound* sound;
            std::unique_ptr<SoundFileReader> soundFileReader;
            bool playLoop;

            StreamChunk* streamChunks;
    };

}
