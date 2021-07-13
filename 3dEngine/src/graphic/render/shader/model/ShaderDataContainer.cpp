#include <cstring>

#include <graphic/render/shader/model/ShaderDataContainer.h>
#include <graphic/setup/GraphicService.h>

namespace urchin {

    ShaderDataContainer::ShaderDataContainer(std::size_t dataSize, const void* ptr) :
            dataSize(dataSize),
            newData({}) {
        this->ptr = malloc(dataSize);
        std::memcpy(this->ptr, ptr, dataSize);

        resetNewDataFlag();
    }

    ShaderDataContainer::ShaderDataContainer(const ShaderDataContainer& src) :
            dataSize(src.dataSize),
            newData(src.newData) {
        this->ptr = malloc(dataSize);
        std::memcpy(this->ptr, src.ptr, dataSize);
    }

    ShaderDataContainer::~ShaderDataContainer() {
        free(ptr);
    }

    void ShaderDataContainer::updateData(const void* newDataPtr) {
        std::memcpy(this->ptr, newDataPtr, dataSize);
        resetNewDataFlag();
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

        return newData[frameIndex];
    }

    void ShaderDataContainer::newDataAck(uint32_t frameIndex) {
        newData[frameIndex] = false;
    }

    void ShaderDataContainer::resetNewDataFlag() {
        std::fill(newData.begin(), newData.end(), true);
    }

}
