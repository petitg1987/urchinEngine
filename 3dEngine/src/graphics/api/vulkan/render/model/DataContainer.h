#pragma once

#include <cstdlib>
#include <array>

#include <graphics/api/vulkan/render/model/DataType.h>
#include <graphics/api/vulkan/render/model/VariableType.h>

namespace urchin {

    class DataContainer {
        public:
            DataContainer(DataType, VariableType, std::size_t, const void*);
            DataContainer(const DataContainer&);
            DataContainer& operator=(const DataContainer&) = delete;
            DataContainer(DataContainer&&) noexcept;
            DataContainer& operator=(DataContainer&&) noexcept = delete;
            ~DataContainer();

            void replaceData(std::size_t, const void*);

            const void* getData() const;
            VariableType getVariableType() const;
            DataType getDataType() const;
            std::size_t getDataCount() const;
            std::size_t getDataSize() const;
            std::size_t getBufferSize() const;

            bool hasNewData(uint32_t) const;
            void markDataAsProcessed(uint32_t);
            void markDataAsProcessed();

        private:
            unsigned int getTypeSize() const;
            unsigned int getVariableSize() const;
            void markDataAsNew();

            DataType dataType;
            VariableType variableType;
            std::size_t dataCount;
            void* ptr;

            static constexpr uint32_t MAX_FRAMES = 6;
            std::array<bool, MAX_FRAMES> bHasNewData;
    };

}
