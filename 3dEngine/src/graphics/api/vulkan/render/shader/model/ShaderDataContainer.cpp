#include <cstring>

#include <graphics/api/vulkan/render/shader/model/ShaderDataContainer.h>
#include <graphics/api/vulkan/setup/GraphicService.h>

namespace urchin {

    ShaderDataContainer::ShaderDataContainer(std::size_t dataSize, const void* ptr) :
            dataSize(dataSize),
            bHasNewData({}) {
        this->ptr = ::operator new(dataSize);
        std::memcpy(this->ptr, ptr, dataSize);

        markDataAsNew();
    }

    ShaderDataContainer::ShaderDataContainer(const ShaderDataContainer& src) :
            dataSize(src.dataSize),
            bHasNewData(src.bHasNewData) {
        this->ptr = ::operator new(dataSize);
        std::memcpy(this->ptr, src.ptr, dataSize);
    }

    ShaderDataContainer::~ShaderDataContainer() {
        ::operator delete(ptr);
    }

    void ShaderDataContainer::updateData(const void* newDataPtr) {
        std::memcpy(this->ptr, newDataPtr, dataSize);
        markDataAsNew();
    }

    std::size_t ShaderDataContainer::getDataSize() const {
        return dataSize;
    }

    void* ShaderDataContainer::getData() const {
        return ptr;
    }

    bool ShaderDataContainer::hasNewData(uint32_t frameIndex) const {
        if (frameIndex >= MAX_FRAMES) {
            throw std::runtime_error("Number of frames higher than expected: " + std::to_string(frameIndex));
        }
        return bHasNewData[frameIndex];
    }

    void ShaderDataContainer::markDataAsProcessed(uint32_t frameIndex) {
        bHasNewData[frameIndex] = false;
    }

    void ShaderDataContainer::markDataAsProcessed() {
        std::ranges::fill(bHasNewData, false);
    }

    void ShaderDataContainer::markDataAsNew() {
        std::ranges::fill(bHasNewData, true);
    }

}
