#include <stdexcept>
#include <cstring>
#include <algorithm>

#include <graphics/api/vulkan/render/model/DataContainer.h>

namespace urchin {

    DataContainer::DataContainer(DataType dataType, VariableType variableType, std::size_t dataCount, const void* ptr) :
            dataType(dataType),
            variableType(variableType),
            dataCount(dataCount),
            bHasNewData({}) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());

        markDataAsNew();
    }

    DataContainer::DataContainer(const DataContainer& src) :
            dataType(src.dataType),
            variableType(src.variableType),
            dataCount(src.dataCount),
            bHasNewData(src.bHasNewData) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, src.ptr, getBufferSize());
    }

    DataContainer::DataContainer(DataContainer&& src) noexcept :
            dataType(src.dataType),
            variableType(src.variableType),
            dataCount(src.dataCount),
            ptr(src.ptr),
            bHasNewData(src.bHasNewData) {
        src.ptr = nullptr;
    }

    DataContainer::~DataContainer() {
        ::operator delete(ptr);
    }

    void DataContainer::replaceData(std::size_t dataCount, const void* ptr) {
        static constexpr std::size_t MAX_MEMORY_RATIO = 5; //to avoid too much memory consumption for nothing
        if (this->dataCount < dataCount || this->dataCount > dataCount * MAX_MEMORY_RATIO) {
            ::operator delete(this->ptr);

            this->dataCount = dataCount;
            this->ptr = ::operator new(getBufferSize());
        } else {
            this->dataCount = dataCount;
        }
        std::memcpy(this->ptr, ptr, getBufferSize());

        markDataAsNew();
    }

    const void* DataContainer::getData() const {
        return ptr;
    }

    VariableType DataContainer::getVariableType() const {
        return variableType;
    }

    DataType DataContainer::getDataType() const {
        return dataType;
    }

    /**
     * @return Number of data (= vector/point). Example: a quad has 4 data/points.
     */
    std::size_t DataContainer::getDataCount() const {
        return dataCount;
    }

    /**
     * @return Memory size of one data (= vector/point)
     */
    std::size_t DataContainer::getDataSize() const {
        return getTypeSize() * getVariableSize();
    }

    /**
     * @return Memory size of all the data (= vector/point)
     */
    std::size_t DataContainer::getBufferSize() const {
        return getTypeSize() * getVariableSize() * dataCount;
    }

    VkFormat DataContainer::getVulkanFormat(unsigned int& repeatCount) const {
        if (dataType == DataType::FLOAT) {
            if (variableType == VariableType::FLOAT) {
                repeatCount = 1;
                return VK_FORMAT_R32_SFLOAT;
            } else if (variableType == VariableType::VEC2) {
                repeatCount = 1;
                return VK_FORMAT_R32G32_SFLOAT;
            } else if (variableType == VariableType::VEC3) {
                repeatCount = 1;
                return VK_FORMAT_R32G32B32_SFLOAT;
            } else if (variableType == VariableType::MAT4) {
                repeatCount = 4;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            } else if (variableType == VariableType::TWO_MAT4) {
                repeatCount = 8;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            throw std::runtime_error("Unknown variable type: " + std::to_string((int)variableType));
        }
        throw std::runtime_error("Unknown data type: " + std::to_string((int)dataType));
    }

    unsigned int DataContainer::getTypeSize() const {
        if (dataType == DataType::FLOAT) {
            return sizeof(float);
        }
        throw std::runtime_error("Unknown data type: " + std::to_string((int)dataType));
    }

    unsigned int DataContainer::getVariableSize() const {
        if (variableType == VariableType::FLOAT) {
            return 1;
        } else if (variableType == VariableType::VEC2) {
            return 2;
        } else if (variableType == VariableType::VEC3) {
            return 3;
        } else if (variableType == VariableType::MAT4) {
            return 16;
        } else if (variableType == VariableType::TWO_MAT4) {
            return 32;
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string((int)variableType));
    }

    bool DataContainer::hasNewData(uint32_t frameIndex) const {
        if (frameIndex >= MAX_FRAMES) {
            throw std::runtime_error("Number of frames higher than expected: " + std::to_string(frameIndex));
        }
        return bHasNewData[frameIndex];
    }

    void DataContainer::markDataAsProcessed(uint32_t frameIndex) {
        bHasNewData[frameIndex] = false;
    }

    void DataContainer::markDataAsProcessed() {
        std::ranges::fill(bHasNewData, false);
    }

    void DataContainer::markDataAsNew() {
        std::ranges::fill(bHasNewData, true);
    }
}