#include <cassert>

#include <player/stream/StreamUpdateTask.h>

namespace urchin {

    /**
     * @param streamChunks Stream chunks (uninitialized)
     */
    StreamUpdateTask::StreamUpdateTask(const Sound* sound, unsigned int nbStreamChunks, bool playLoop) :
            sound(sound),
            soundFileReader(std::make_unique<SoundFileReader>(sound->getFilename())),
            playLoop(playLoop) {
        this->streamChunks.resize(nbStreamChunks, {});
    }

    ALuint StreamUpdateTask::getSourceId() const {
        return sound->getSourceId();
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool StreamUpdateTask::isSourceStopped() const {
        return sound->isStopped();
    }

    const SoundFileReader& StreamUpdateTask::getSoundFileReader() const {
        return *soundFileReader;
    }

    bool StreamUpdateTask::isPlayLoop() const {
        return playLoop;
    }

    std::string StreamUpdateTask::getSoundFilename() const {
        return sound->getFilename();
    }

    StreamChunk& StreamUpdateTask::getStreamChunk(unsigned int chunkId) {
        assert(chunkId < streamChunks.size());
        return streamChunks[chunkId];
    }

}
