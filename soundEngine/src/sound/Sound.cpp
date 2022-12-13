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
        const PreLoadedChunks& plc = chunkPreLoader.preLoad(filename);
        preLoadedChunks = plc.chunks;
        chunkMaxSize = plc.chunkMaxSize;
    }

    bool Sound::hasPreLoadedChunks() const {
        return !preLoadedChunks.empty();
    }

    std::vector<int16_t> Sound::getPreLoadedChunk(std::size_t chunkIndex, bool loop) const {
        assert(chunkIndex < preLoadedChunks.size());
        if (loop) {
            if (preLoadedChunks[chunkIndex].size() == chunkMaxSize) {
                return preLoadedChunks[chunkIndex];
            } else {
                std::vector<int16_t> preLoadedChunkLoop = preLoadedChunks[chunkIndex];
                preLoadedChunkLoop.resize(chunkMaxSize);
                std::size_t readIndex = 0;
                for (std::size_t writeIndex = preLoadedChunks[chunkIndex].size(); writeIndex < preLoadedChunkLoop.size(); ++writeIndex) {
                    preLoadedChunkLoop[writeIndex] = preLoadedChunks[0][readIndex];
                    readIndex++;
                    readIndex = readIndex % preLoadedChunks[0].size();
                }
                return preLoadedChunkLoop;
            }
        } else {
            return preLoadedChunks[chunkIndex];
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
