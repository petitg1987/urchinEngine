#pragma once

#include <cstdlib>
#include <array>

namespace urchin {

    class ShaderDataContainer {
        public:
            ShaderDataContainer(std::size_t, const void*);
            ShaderDataContainer(const ShaderDataContainer&);
            ShaderDataContainer& operator=(const ShaderDataContainer&) = delete;
            ~ShaderDataContainer();

            void updateData(const void*);
            void* getData() const;
            std::size_t getDataSize() const;

            bool hasNewData(uint32_t) const;
            void markDataAsProcessed(uint32_t);
            void markDataAsProcessed();

        private:
            void markDataAsNew();

            std::size_t dataSize;
            void* ptr;

            static constexpr uint32_t MAX_DATA = 6;
            std::array<bool, MAX_DATA> bHasNewData;
    };

}
