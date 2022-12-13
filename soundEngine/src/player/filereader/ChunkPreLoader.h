#pragma once

namespace urchin {

    struct PreLoadedChunks {
        std::vector<std::vector<int16_t>> chunks;
        std::size_t chunkMaxSize = 0;
    };

    class ChunkPreLoader {
        public:
            ChunkPreLoader();

            const PreLoadedChunks& preLoad(const std::string&);

        private:
            unsigned int nbChunkBuffer;
            unsigned int chunkSizeInMs;

            std::map<std::string, PreLoadedChunks, std::less<>> chunksCache;
    };

}