#ifndef URCHINENGINE_DATACONTAINER_H
#define URCHINENGINE_DATACONTAINER_H

#include <cstdlib>
#include <vulkan/vulkan.h>

#include "graphic/render/model/DataType.h"
#include "graphic/render/model/DataDimension.h"

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

            bool hasNewData() const;
            void newDataAck(std::size_t);
            void resetNewDataFlag();

        private:
            unsigned int getTypeSize() const;
            unsigned int getDimensionSize() const;

            DataType dataType;
            DataDimension dataDimension;
            std::size_t dataCount;
            void* ptr;

            bool newData;
            uint32_t newDataTotalUpdate;
    };

}

#endif
