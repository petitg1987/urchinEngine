#ifndef URCHINENGINE_SHADERDATACONTAINER_H
#define URCHINENGINE_SHADERDATACONTAINER_H

#include <cstdlib>
#include <cstdint>

namespace urchin {

    class ShaderDataContainer {
        public:
            ShaderDataContainer(std::size_t, const void*);
            ShaderDataContainer(const ShaderDataContainer&);
            ~ShaderDataContainer();

            void updateData(const void*);
            void* getData() const;
            std::size_t getDataSize() const;

            bool hasNewData() const;
            void newDataAck(std::size_t);
            void resetNewDataFlag();

        private:
            std::size_t dataSize;
            void* ptr;

            bool newData;
            uint32_t newDataTotalUpdate;
    };

}

#endif
