#include <cstring>

#include <graphics/render/data/IndexContainer.h>

namespace urchin {

    IndexContainer::IndexContainer(std::size_t indicesCount, const uint32_t* ptr) :
            indicesCount(indicesCount) {
        this->ptr = (uint32_t*)operator new(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());
    }

    IndexContainer::IndexContainer(const IndexContainer& src) :
            indicesCount(src.indicesCount) {
        this->ptr = (uint32_t*)operator new(getBufferSize());
        std::memcpy(this->ptr, src.ptr, getBufferSize());
    }

    IndexContainer::IndexContainer(IndexContainer&& src) noexcept :
            indicesCount(src.indicesCount),
            ptr(src.ptr) {
        src.ptr = nullptr;
    }

    IndexContainer::~IndexContainer() {
        operator delete(ptr);
    }

    const uint32_t* IndexContainer::getIndices() const {
        return ptr;
    }

    /**
     * @return Number of indices
     */
    std::size_t IndexContainer::getIndicesCount() const {
        return indicesCount;
    }

    /**
     * @return Memory size of one index
     */
    std::size_t IndexContainer::getIndicesSize() {
        return sizeof(uint32_t);
    }

    /**
     * @return Memory size of all the indices
     */
    std::size_t IndexContainer::getBufferSize() const {
        return getIndicesSize() * indicesCount;
    }

}
