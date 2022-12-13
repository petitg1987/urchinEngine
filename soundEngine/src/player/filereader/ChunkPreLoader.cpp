#include <player/filereader/ChunkPreLoader.h>
#include "SoundFileReader.h"

namespace urchin {

    ChunkPreLoader::ChunkPreLoader() :
            nbChunkBuffer(ConfigService::instance().getUnsignedIntValue("player.numberOfStreamBuffer")),
            chunkSizeInMs(ConfigService::instance().getUnsignedIntValue("player.streamChunkSizeInMs")),
            chunksCache(ConfigService::instance().getUnsignedIntValue("player.preLoadedChunksCacheSize")) {

    }

    PreLoadedChunks ChunkPreLoader::preLoad(const std::string& filename) {
        std::optional<PreLoadedChunks> chunkCached = chunksCache.get(filename);
        if (chunkCached) {
            return chunkCached.value();
        }

        SoundFileReader soundFileReader(filename);

        PreLoadedChunks preLoadedChunks;
        preLoadedChunks.chunks.resize(nbChunkBuffer);
        preLoadedChunks.chunkMaxSize = (std::size_t)((float)soundFileReader.getSampleRate() * (float)soundFileReader.getNumberOfChannels() * ((float)chunkSizeInMs / 1000.0f));

        for (std::size_t chunkIndex = 0; chunkIndex < nbChunkBuffer; ++chunkIndex) {
            preLoadedChunks.chunks[chunkIndex].resize(preLoadedChunks.chunkMaxSize);

            unsigned int numSamplesRead = 0;
            soundFileReader.readNextChunk(preLoadedChunks.chunks[chunkIndex], numSamplesRead, false);
            preLoadedChunks.chunks[chunkIndex].resize(numSamplesRead);
        }

        chunksCache.put(filename, preLoadedChunks);
        return preLoadedChunks;
    }

}
