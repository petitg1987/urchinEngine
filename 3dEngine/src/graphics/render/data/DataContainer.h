#pragma once

#include <cstdint>
#include <array>
#include <vector>

#include "graphics/render/data/VariableType.h"

namespace urchin {

    class DataContainer {
        public:
            DataContainer(std::vector<VariableType>, std::size_t, const void*);
            DataContainer(const DataContainer&);
            DataContainer& operator=(const DataContainer&) = delete;
            DataContainer(DataContainer&&) noexcept;
            DataContainer& operator=(DataContainer&&) noexcept = delete;
            ~DataContainer();

            void replaceData(std::size_t, const void*);

            const void* getData() const;
            const std::vector<VariableType>& getVariableTypes() const;
            unsigned int getVariableSize(std::size_t) const;
            unsigned int getVariableSizes() const;
            std::size_t getDataCount() const;
            std::size_t getBufferSize() const;

            bool hasNewData(uint32_t) const;
            void markDataAsProcessed(uint32_t);
            void markDataAsProcessed();

        private:
            void markDataAsNew();

            std::vector<VariableType> variableTypes;
            std::size_t dataCount;
            void* ptr;

            static constexpr uint32_t MAX_DATA = 6;
            std::array<bool, MAX_DATA> bHasNewData;
    };

}
