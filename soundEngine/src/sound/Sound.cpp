#include <utility>

#include <sound/Sound.h>
#include <player/filereader/SoundFileReader.h>

namespace urchin {

    /**
     * @param initialVolume Initial sound volume (0.0=minimum volume, 1.0=original volume). Volume can be higher to 1.0.
     */
    Sound::Sound(std::string filename, SoundCategory category, float initialVolume) :
            filename(std::move(filename)),
            category(category),
            initialVolume(initialVolume) {
        preLoadChunks();
    }

    void Sound::preLoadChunks() {
        SoundFileReader soundFileReader(filename);

        unsigned int nbChunkBuffer = ConfigService::instance().getUnsignedIntValue("player.numberOfStreamBuffer");
        preLoadedChunks.resize(nbChunkBuffer);

        unsigned int chunkSizeInMs = ConfigService::instance().getUnsignedIntValue("player.streamChunkSizeInMs");
        float bufferSize = (float) soundFileReader.getSampleRate() * (float) soundFileReader.getNumberOfChannels() * ((float) chunkSizeInMs / 1000.0f);

        for (std::size_t chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
            preLoadedChunks[chunkIndex].resize((std::size_t) bufferSize);

            unsigned int numSamplesRead = 0;
            soundFileReader.readNextChunk(preLoadedChunks[chunkIndex], numSamplesRead, false);
            preLoadedChunks[chunkIndex].resize(numSamplesRead);
        }
    }

    const std::string& Sound::getFilename() const {
        return filename;
    }

    Sound::SoundCategory Sound::getSoundCategory() const {
        return category;
    }

    float Sound::getInitialVolume() const {
        return initialVolume;
    }

    const std::vector<int16_t>& Sound::getPreLoadedChunk(std::size_t chunkIndex) const {
        assert(chunkIndex < preLoadedChunks.size());
        return preLoadedChunks[chunkIndex];
    }


}
