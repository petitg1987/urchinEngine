#include "player/stream/StreamUpdateTask.h"

namespace urchin {

    /**
     * @param streamChunks Stream chunks (uninitialized)
     */
    StreamUpdateTask::StreamUpdateTask(const Sound *sound, StreamChunk *streamChunks, bool playLoop) :
        sound(sound),
        soundFileReader(new SoundFileReader(sound->getFilename())),
        playLoop(playLoop),
        streamChunks(streamChunks) {

    }

    StreamUpdateTask::~StreamUpdateTask() {
        delete soundFileReader;
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

    SoundFileReader *StreamUpdateTask::getSoundFileReader() {
        return soundFileReader;
    }

    bool StreamUpdateTask::isPlayLoop() const {
        return playLoop;
    }

    std::string StreamUpdateTask::getSoundFilename() const {
        return sound->getFilename();
    }

    StreamChunk &StreamUpdateTask::getStreamChunk(unsigned int chunkId) {
        return streamChunks[chunkId];
    }

    StreamChunk *StreamUpdateTask::getStreamChunks() {
        return streamChunks;
    }

}
