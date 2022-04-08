#include <cassert>

#include <player/stream/StreamUpdateTask.h>

namespace urchin {

    /**
     * @param streamChunks Stream chunks (uninitialized)
     */
    StreamUpdateTask::StreamUpdateTask(const AudioStreamPlayer& audioStreamPlayer, unsigned int nbStreamChunks, bool playLoop) :
            audioStreamPlayer(audioStreamPlayer),
            soundFileReader(SoundFileReader(audioStreamPlayer.getSound().getFilename())),
            playLoop(playLoop),
            initialReadSamples(0) {
        this->streamChunks.resize(nbStreamChunks, {});

        if (soundFileReader.getNumberOfChannels() != 1 && audioStreamPlayer.getSound().getSoundType() == Sound::SoundType::SPATIAL) {
            throw std::runtime_error("Spatial sound " + audioStreamPlayer.getSound().getFilename() + " must be a mono sound");
        }
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

    StreamChunk& StreamUpdateTask::getStreamChunk(unsigned int chunkIndex) {
        assert(chunkIndex < streamChunks.size());
        return streamChunks[chunkIndex];
    }

    void StreamUpdateTask::setInitialReadSamples(unsigned int initialReadSamples) {
        this->initialReadSamples = initialReadSamples;
    }

    void StreamUpdateTask::initializeReadCursor() {
        if (initialReadSamples != 0) {
            soundFileReader.advanceReadCursor(initialReadSamples, playLoop);
            initialReadSamples = 0;
        }
    }

}
