#include <cstring>

#include <graphic/render/shader/model/ShaderDataContainer.h>

namespace urchin {

    ShaderDataContainer::ShaderDataContainer(std::size_t dataSize, const void* ptr) :
            dataSize(dataSize),
            newData(true),
            newDataTotalUpdate(0) {
        this->ptr = malloc(dataSize);
        std::memcpy(this->ptr, ptr, dataSize);
    }

    ShaderDataContainer::ShaderDataContainer(const ShaderDataContainer& src) :
            dataSize(src.dataSize),
            newData(src.newData),
            newDataTotalUpdate(src.newDataTotalUpdate) {
        this->ptr = malloc(dataSize);
        std::memcpy(this->ptr, src.ptr, dataSize);
    }

    ShaderDataContainer::~ShaderDataContainer() {
        free(ptr);
    }

    void ShaderDataContainer::updateData(const void* newDataPtr) {
        std::memcpy(this->ptr, newDataPtr, dataSize);
        newData = true;
    }

    std::size_t ShaderDataContainer::getDataSize() const {
        return dataSize;
    }

    void* ShaderDataContainer::getData() const {
        return ptr;
    }

    bool ShaderDataContainer::hasNewData() const {
        return newData;
    }

    void ShaderDataContainer::newDataAck(std::size_t nbFramebuffer) {
        newDataTotalUpdate++;
        if (nbFramebuffer == newDataTotalUpdate) {
            newDataTotalUpdate = 0;
            newData = false;
        }
    }

    void ShaderDataContainer::resetNewDataFlag() {
        newDataTotalUpdate = 0;
        newData = true;
    }

}
