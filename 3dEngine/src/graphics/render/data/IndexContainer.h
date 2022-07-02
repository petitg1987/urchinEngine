#pragma once

#include <cstdint>

namespace urchin {

    class IndexContainer {
        public:
            IndexContainer(std::size_t, const uint32_t*);
            IndexContainer(const IndexContainer&);
            IndexContainer& operator=(const IndexContainer&) = delete;
            IndexContainer(IndexContainer&&) noexcept;
            IndexContainer& operator=(IndexContainer&&) noexcept = delete;
            ~IndexContainer();

            const uint32_t* getIndices() const;
            std::size_t getIndicesCount() const;
            static std::size_t getIndicesSize();
            std::size_t getBufferSize() const;

        private:
            std::size_t indicesCount;
            uint32_t* ptr;
    };

}
