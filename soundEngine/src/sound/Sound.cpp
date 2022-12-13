#include <utility>

#include <sound/Sound.h>
#include <player/filereader/SoundFileReader.h>

namespace urchin {

    /**
     * @param initialVolume Initial sound volume (0.0=minimum volume, 1.0=original volume). Volume can be higher to 1.0.
     */
    Sound::Sound(std::string filename, SoundCategory category, float initialVolume) :
            category(category),
            initialVolume(initialVolume) {
        this->filename = FileUtil::isAbsolutePath(filename) ? std::move(filename) : FileSystem::instance().getResourcesDirectory() + std::move(filename);
    }

    void Sound::preLoadChunks(ChunkPreLoader& chunkPreLoader) {
        preLoadedChunks = chunkPreLoader.preLoad(filename);
    }

    bool Sound::hasPreLoadedChunks() const {
        return !preLoadedChunks.chunks.empty();
    }

    std::vector<int16_t> Sound::getPreLoadedChunk(std::size_t chunkIndex, bool loop) const {
        assert(chunkIndex < preLoadedChunks.chunks.size());
        if (loop) {
            if (preLoadedChunks.chunks[chunkIndex].size() == preLoadedChunks.chunkMaxSize) {
                return preLoadedChunks.chunks[chunkIndex];
            } else {
                std::vector<int16_t> preLoadedChunkLoop = preLoadedChunks.chunks[chunkIndex];
                preLoadedChunkLoop.resize(preLoadedChunks.chunkMaxSize);
                std::size_t readIndex = 0;
                for (std::size_t writeIndex = preLoadedChunks.chunks[chunkIndex].size(); writeIndex < preLoadedChunkLoop.size(); ++writeIndex) {
                    preLoadedChunkLoop[writeIndex] = preLoadedChunks.chunks[0][readIndex];
                    readIndex++;
                    readIndex = readIndex % preLoadedChunks.chunks[0].size();
                }
                return preLoadedChunkLoop;
            }
        } else {
            return preLoadedChunks.chunks[chunkIndex];
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

}
