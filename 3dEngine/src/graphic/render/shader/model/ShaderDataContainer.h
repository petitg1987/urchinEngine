#pragma once

#include <cstdlib>
#include <cstdint>
#include <array>

namespace urchin {

    class ShaderDataContainer {
        public:
            ShaderDataContainer(std::size_t, const void*);
            ShaderDataContainer(const ShaderDataContainer&);
            ~ShaderDataContainer();

            void updateData(const void*);
            void* getData() const;
            std::size_t getDataSize() const;

            bool hasNewData(uint32_t) const; //TODO rename all methods
            void newDataAck(uint32_t);
            void newDataAck();
            void resetNewDataFlag();

        private:
            std::size_t dataSize;
            void* ptr;

            static constexpr uint32_t MAX_FRAMES = 6;
            std::array<bool, MAX_FRAMES> newData;
    };

}
