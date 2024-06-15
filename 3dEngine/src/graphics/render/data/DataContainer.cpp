#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <utility>

#include <graphics/render/data/DataContainer.h>

namespace urchin {

    DataContainer::DataContainer(std::vector<VariableType> variableTypes, std::size_t dataCount, const void* ptr) :
            variableTypes(std::move(variableTypes)),
            dataCount(dataCount),
            bHasNewData({}) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());

        markDataAsNew();
    }

    DataContainer::DataContainer(const DataContainer& src) :
            variableTypes(src.variableTypes),
            dataCount(src.dataCount),
            bHasNewData(src.bHasNewData) {
        this->ptr = ::operator new(getBufferSize());
        std::memcpy(this->ptr, src.ptr, getBufferSize());
    }

    DataContainer::DataContainer(DataContainer&& src) noexcept :
            variableTypes(std::move(src.variableTypes)),
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

    const std::vector<VariableType>& DataContainer::getVariableTypes() const {
        return variableTypes;
    }

    unsigned int DataContainer::getVariableSize(VariableType variableType) const {
        if (variableType == VariableType::FLOAT) {
            return 1 * sizeof(float);
        } else if (variableType == VariableType::VEC2_FLOAT) {
            return 2 * sizeof(float);
        } else if (variableType == VariableType::VEC3_FLOAT) {
            return 3 * sizeof(float);
        } else if (variableType == VariableType::MAT4_FLOAT) {
            return 16 * sizeof(float);
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string((int)variableType));
    }

    unsigned int DataContainer::getVariablesSize() const {
        unsigned int variablesSize = 0;
        for(VariableType variableType : variableTypes) {
            variablesSize += getVariableSize(variableType);
        }
        return variablesSize;
    }

    /**
     * @return Number of data (= vector/point). Example: a quad has 4 data/points.
     */
    std::size_t DataContainer::getDataCount() const {
        return dataCount;
    }

    /**
     * @return Memory size of all the data (= vector/point)
     */
    std::size_t DataContainer::getBufferSize() const {
        return getVariablesSize() * dataCount;
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