#include <stdexcept>
#include <cstring>

#include <graphic/render/model/DataContainer.h>

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

    DataContainer& DataContainer::operator=(DataContainer&& src) noexcept {
        dataType = src.dataType;
        dataDimension = src.dataDimension;
        dataCount = src.dataCount;

        ::operator delete(ptr);
        ptr = src.ptr;
        src.ptr = nullptr;

        bHasNewData = src.bHasNewData;

        return *this;
    }

    DataContainer::~DataContainer() {
        ::operator delete(ptr);
    }

    const void* DataContainer::getData() const {
        return ptr;
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

    VkFormat DataContainer::getVulkanFormat() const {
        if (dataType == DataType::FLOAT) {
            if (dataDimension == DataDimension::TWO_DIMENSION) {
                return VK_FORMAT_R32G32_SFLOAT;
            } else if (dataDimension == DataDimension::THREE_DIMENSION) {
                return VK_FORMAT_R32G32B32_SFLOAT;
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
        std::fill(bHasNewData.begin(), bHasNewData.end(), false);
    }

    void DataContainer::markDataAsNew() {
        std::fill(bHasNewData.begin(), bHasNewData.end(), true);
    }
}