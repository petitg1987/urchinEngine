#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <utility>

#include "graphics/render/data/DataContainer.h"

namespace urchin {

    DataContainer::DataContainer(std::vector<VariableType> variableTypes, std::size_t dataCount, const void* ptr) :
            variableTypes(std::move(variableTypes)),
            dataCount(dataCount),
            bHasNewData({}) {
        this->ptr = operator new(getBufferSize());
        std::memcpy(this->ptr, ptr, getBufferSize());

        markDataAsNew();
    }

    DataContainer::DataContainer(const DataContainer& src) :
            variableTypes(src.variableTypes),
            dataCount(src.dataCount),
            bHasNewData(src.bHasNewData) {
        this->ptr = operator new(getBufferSize());
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
        operator delete(ptr);
    }

    void DataContainer::replaceData(std::size_t dataCount, const void* ptr) {
        static constexpr std::size_t MAX_MEMORY_RATIO = 5; //to avoid too much memory consumption for nothing
        if (this->dataCount < dataCount || this->dataCount > dataCount * MAX_MEMORY_RATIO) {
            operator delete(this->ptr);

            this->dataCount = dataCount;
            this->ptr = operator new(getBufferSize());
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

    unsigned int DataContainer::getVariableSize(std::size_t variableTypeIndex) const {
        assert(variableTypes.size() > variableTypeIndex);

        if (variableTypes[variableTypeIndex] == VariableType::FLOAT) {
            return sizeof(float) * 1;
        } else if (variableTypes[variableTypeIndex] == VariableType::VEC2_FLOAT) {
            return sizeof(float) * 2;
        } else if (variableTypes[variableTypeIndex] == VariableType::VEC3_FLOAT) {
            return sizeof(float) * 3;
        } else if (variableTypes[variableTypeIndex] == VariableType::VEC4_FLOAT) {
            return sizeof(float) * 4;
        } else if (variableTypes[variableTypeIndex] == VariableType::MAT4_FLOAT) {
            return sizeof(float) * 16;
        }
        throw std::runtime_error("Unknown data dimension: " + std::to_string((int)variableTypes[variableTypeIndex]));
    }

    unsigned int DataContainer::getVariableSizes() const {
        unsigned int variableSizes = 0;
        for (std::size_t variableTypeIndex = 0; variableTypeIndex < variableTypes.size(); ++variableTypeIndex) {
            variableSizes += getVariableSize(variableTypeIndex);
        }
        return variableSizes;
    }

    std::size_t DataContainer::getDataCount() const {
        return dataCount;
    }

    std::size_t DataContainer::getBufferSize() const {
        return getVariableSizes() * dataCount;
    }

    bool DataContainer::hasNewData(uint32_t framebufferIndex) const {
        if (framebufferIndex >= MAX_DATA) {
            throw std::runtime_error("Number of framebuffers higher than expected: " + std::to_string(framebufferIndex));
        }
        return bHasNewData[framebufferIndex];
    }

    void DataContainer::markDataAsProcessed(uint32_t framebufferIndex) {
        bHasNewData[framebufferIndex] = false;
    }

    void DataContainer::markDataAsProcessed() {
        std::ranges::fill(bHasNewData, false);
    }

    void DataContainer::markDataAsNew() {
        std::ranges::fill(bHasNewData, true);
    }
}