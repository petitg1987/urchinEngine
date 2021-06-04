#include <stdexcept>
#include <cstring>

#include <graphic/render/model/DataContainer.h>

namespace urchin {

    DataContainer::DataContainer(DataType dataType, DataDimension dataDimension, std::size_t dataCount, const void* ptr) :
            dataType(dataType),
            dataDimension(dataDimension),
            dataCount(dataCount),
            newData(true),
            newDataTotalUpdate(0) {
        this->ptr = malloc(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());
    }

    DataContainer::DataContainer(const DataContainer& src) :
            dataType(src.dataType),
            dataDimension(src.dataDimension),
            dataCount(src.dataCount),
            newData(src.newData),
            newDataTotalUpdate(src.newDataTotalUpdate) {
        this->ptr = malloc(getBufferSize());
        std::memcpy(this->ptr, src.ptr, getBufferSize());
    }

    DataContainer::DataContainer(DataContainer&& src) noexcept :
            dataType(src.dataType),
            dataDimension(src.dataDimension),
            dataCount(src.dataCount),
            ptr(src.ptr),
            newData(src.newData),
            newDataTotalUpdate(src.newDataTotalUpdate) {
        src.ptr = nullptr;
    }

    DataContainer& DataContainer::operator=(DataContainer&& src) noexcept {
        dataType = src.dataType;
        dataDimension = src.dataDimension;
        dataCount = src.dataCount;

        free(ptr);
        ptr = src.ptr;
        src.ptr = nullptr;

        newData = src.newData;
        newDataTotalUpdate = src.newDataTotalUpdate;

        return *this;
    }

    DataContainer::~DataContainer() {
        free(ptr);
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
            throw std::runtime_error("Unknown data dimension: " + std::to_string(dataDimension));
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int DataContainer::getTypeSize() const {
        if (dataType == DataType::FLOAT) {
            return sizeof(float);
        }
        throw std::runtime_error("Unknown data type: " + std::to_string(dataType));
    }

    unsigned int DataContainer::getDimensionSize() const {
        if (dataDimension == DataDimension::TWO_DIMENSION) {
            return 2;
        } else if (dataDimension == DataDimension::THREE_DIMENSION) {
            return 3;
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string(dataDimension));
    }

    bool DataContainer::hasNewData() const {
        return newData;
    }

    void DataContainer::newDataAck(std::size_t nbFramebuffer) {
        newDataTotalUpdate++;
        if (nbFramebuffer == newDataTotalUpdate) {
            newDataTotalUpdate = 0;
            newData = false;
        }
    }

    void DataContainer::resetNewDataFlag() {
        newDataTotalUpdate = 0;
        newData = true;
    }
}