#include <stdexcept>
#include <cstring>
#include <algorithm>

#include <api/render/model/DataContainer.h>

namespace urchin {

    DataContainer::DataContainer(DataType dataType, DataDimension dataDimension, std::size_t dataCount, const void* ptr) :
            dataType(dataType),
            dataDimension(dataDimension),
            dataCount(dataCount),
            bHasNewData({}) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());

        markDataAsNew();
    }

    DataContainer::DataContainer(const DataContainer& src) :
            dataType(src.dataType),
            dataDimension(src.dataDimension),
            dataCount(src.dataCount),
            bHasNewData(src.bHasNewData) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, src.ptr, getBufferSize());
    }

    DataContainer::DataContainer(DataContainer&& src) noexcept :
            dataType(src.dataType),
            dataDimension(src.dataDimension),
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

    DataDimension DataContainer::getDataDimension() const {
        return dataDimension;
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
        return getTypeSize() * getDimensionSize();
    }

    /**
     * @return Memory size of all the data (= vector/point)
     */
    std::size_t DataContainer::getBufferSize() const {
        return getTypeSize() * getDimensionSize() * dataCount;
    }

    VkFormat DataContainer::getVulkanFormat(unsigned int& repeatCount) const {
        if (dataType == DataType::FLOAT) {
            if (dataDimension == DataDimension::TWO_DIMENSION) {
                repeatCount = 1;
                return VK_FORMAT_R32G32_SFLOAT;
            } else if (dataDimension == DataDimension::THREE_DIMENSION) {
                repeatCount = 1;
                return VK_FORMAT_R32G32B32_SFLOAT;
            } else if (dataDimension == DataDimension::DIM_16) {
                repeatCount = 4;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            } else if (dataDimension == DataDimension::DIM_32) {
                repeatCount = 8;
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            }
            throw std::runtime_error("Unknown data dimension: " + std::to_string((int)dataDimension));
        }
        throw std::runtime_error("Unknown data type: " + std::to_string((int)dataType));
    }

    unsigned int DataContainer::getTypeSize() const {
        if (dataType == DataType::FLOAT) {
            return sizeof(float);
        }
        throw std::runtime_error("Unknown data type: " + std::to_string((int)dataType));
    }

    unsigned int DataContainer::getDimensionSize() const {
        if (dataDimension == DataDimension::TWO_DIMENSION) {
            return 2;
        } else if (dataDimension == DataDimension::THREE_DIMENSION) {
            return 3;
        } else if (dataDimension == DataDimension::DIM_16) {
            return 16;
        } else if (dataDimension == DataDimension::DIM_32) {
            return 32;
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string((int)dataDimension));
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