#include <cassert>

#include <player/stream/StreamUpdateTask.h>

namespace urchin {

    /**
     * @param streamChunks Stream chunks (uninitialized)
     */
    StreamUpdateTask::StreamUpdateTask(const AudioStreamPlayer& audioStreamPlayer, unsigned int nbStreamChunks, bool playLoop) :
            audioStreamPlayer(audioStreamPlayer),
            soundFileReader(SoundFileReader(audioStreamPlayer.getSound().getFilename())),
            playLoop(playLoop) {
        this->streamChunks.resize(nbStreamChunks, {});
    }

    ALuint StreamUpdateTask::getSourceId() const {
        return audioStreamPlayer.getSourceId();
    }

    /**
     * @return True if source is stopped (not playing, not in pause)
     */
    bool StreamUpdateTask::isSourceStopped() const {
        return audioStreamPlayer.isStopped();
    }

    const SoundFileReader& StreamUpdateTask::getSoundFileReader() const {
        return soundFileReader;
    }

    bool StreamUpdateTask::isPlayLoop() const {
        return playLoop;
    }

    std::string StreamUpdateTask::getSoundFilename() const {
        return audioStreamPlayer.getSound().getFilename();
    }

    StreamChunk& StreamUpdateTask::getStreamChunk(unsigned int chunkId) {
        assert(chunkId < streamChunks.size());
        return streamChunks[chunkId];
    }

}
