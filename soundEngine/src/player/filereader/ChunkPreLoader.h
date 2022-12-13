#pragma once

namespace urchin {

    struct PreLoadedChunks {
        std::vector<std::vector<int16_t>> chunks;
        std::size_t chunkMaxSize = 0;
    };

    class ChunkPreLoader {
        public:
            ChunkPreLoader();

            PreLoadedChunks preLoad(const std::string&) const;

        private:
            unsigned int nbChunkBuffer;
            unsigned int chunkSizeInMs;
    };

}