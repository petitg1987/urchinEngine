#include <cstring>

#include "graphics/render/shader/ShaderDataContainer.h"

namespace urchin {

    ShaderDataContainer::ShaderDataContainer(std::size_t dataSize, const void* ptr) :
            dataSize(dataSize),
            bHasNewData({}),
            updatedDataSize(dataSize) {
        this->ptr = operator new(dataSize);
        std::memcpy(this->ptr, ptr, dataSize);

        markDataAsNew(dataSize);
    }

    ShaderDataContainer::ShaderDataContainer(const ShaderDataContainer& src) :
            dataSize(src.dataSize),
            bHasNewData(src.bHasNewData),
            updatedDataSize(src.updatedDataSize) {
        this->ptr = operator new(dataSize);
        std::memcpy(this->ptr, src.ptr, dataSize);
    }

    ShaderDataContainer::~ShaderDataContainer() {
        operator delete(ptr);
    }

    void ShaderDataContainer::updateData(const void* newDataPtr) {
        if (std::memcmp(this->ptr, newDataPtr, dataSize)) {
            std::memcpy(this->ptr, newDataPtr, dataSize);
            markDataAsNew(dataSize);
        }
    }

    void ShaderDataContainer::updatePartialData(std::size_t dataUpdateSize, const void* newDataPtr) {
        assert(dataUpdateSize <= dataSize);
        if (std::memcmp(this->ptr, newDataPtr, dataUpdateSize)) {
            std::memcpy(this->ptr, newDataPtr, dataUpdateSize);
            markDataAsNew(dataUpdateSize);
        }
    }

    std::size_t ShaderDataContainer::getDataSize() const {
        return dataSize;
    }

    std::size_t ShaderDataContainer::getUpdatedDataSize() const {
        return updatedDataSize;
    }

    void* ShaderDataContainer::getData() const {
        return ptr;
    }

    bool ShaderDataContainer::hasNewData(uint32_t framebufferIndex) const {
        if (framebufferIndex >= MAX_DATA) {
            throw std::runtime_error("Number of framebuffer higher than expected: " + std::to_string(framebufferIndex));
        }
        return bHasNewData[framebufferIndex];
    }

    void ShaderDataContainer::markDataAsProcessed(uint32_t framebufferIndex) {
        bHasNewData[framebufferIndex] = false;
    }

    void ShaderDataContainer::markDataAsProcessed() {
        std::ranges::fill(bHasNewData, false);
    }

    void ShaderDataContainer::markDataAsNew(std::size_t dataUpdateSize) {
        std::ranges::fill(bHasNewData, true);
        this->updatedDataSize = dataUpdateSize;
    }

}
