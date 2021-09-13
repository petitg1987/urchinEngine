#pragma once

#include <cstdlib>
#include <array>
#include <vulkan/vulkan.h>

#include <graphic/render/model/DataType.h>
#include <graphic/render/model/DataDimension.h>

namespace urchin {

    class DataContainer {
        public:
            DataContainer(DataType, DataDimension, std::size_t, const void*);
            DataContainer(const DataContainer&);
            DataContainer(DataContainer&&) noexcept;
            DataContainer& operator=(DataContainer&&) noexcept;
            ~DataContainer();

            const void* getData() const;
            std::size_t getDataCount() const;
            std::size_t getDataSize() const;
            std::size_t getBufferSize() const;

            VkFormat getVulkanFormat() const;

            bool hasNewData(uint32_t) const;
            void markDataAsProcessed(uint32_t);
            void markDataAsProcessed();

        private:
            unsigned int getTypeSize() const;
            unsigned int getDimensionSize() const;
            void markDataAsNew();

            DataType dataType;
            DataDimension dataDimension;
            std::size_t dataCount;
            void* ptr;

            static constexpr uint32_t MAX_FRAMES = 6;
            std::array<bool, MAX_FRAMES> bHasNewData;
    };

}
