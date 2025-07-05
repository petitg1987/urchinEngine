#include <cstring>

#include <graphics/render/shader/ShaderDataContainer.h>

namespace urchin {

    ShaderDataContainer::ShaderDataContainer(std::size_t dataSize, const void* ptr) :
            dataSize(dataSize),
            bHasNewData({}) {
        this->ptr = operator new(dataSize);
        std::memcpy(this->ptr, ptr, dataSize);

        markDataAsNew();
    }

    ShaderDataContainer::ShaderDataContainer(const ShaderDataContainer& src) :
            dataSize(src.dataSize),
            bHasNewData(src.bHasNewData) {
        this->ptr = operator new(dataSize);
        std::memcpy(this->ptr, src.ptr, dataSize);
    }

    ShaderDataContainer::~ShaderDataContainer() {
        operator delete(ptr);
    }

    void ShaderDataContainer::updateData(const void* newDataPtr) {
        if (std::memcmp(this->ptr, newDataPtr, dataSize)) {
            std::memcpy(this->ptr, newDataPtr, dataSize);
            markDataAsNew();
        }
    }

    std::size_t ShaderDataContainer::getDataSize() const {
        return dataSize;
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

    void ShaderDataContainer::markDataAsNew() {
        std::ranges::fill(bHasNewData, true);
    }

}
