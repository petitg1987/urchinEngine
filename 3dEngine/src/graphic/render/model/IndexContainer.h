#pragma once

#include <cstdint>

namespace urchin {

    class IndexContainer {
        public:
            IndexContainer(std::size_t, const uint32_t*, bool hasPrimitiveRestartIndex = false);
            IndexContainer(const IndexContainer&);
            IndexContainer(IndexContainer&&) noexcept;
            ~IndexContainer();

            const uint32_t* getIndices() const;
            std::size_t getIndicesCount() const;
            static std::size_t getIndicesSize();
            std::size_t getBufferSize() const;

            bool hasPrimitiveRestartIndex() const;

        private:
            std::size_t indicesCount;
            uint32_t* ptr;
            bool bHasPrimitiveRestartIndex;
    };

}
