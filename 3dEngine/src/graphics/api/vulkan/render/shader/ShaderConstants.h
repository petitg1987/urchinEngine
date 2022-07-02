#pragma once

#include <vector>

namespace urchin {

    class ShaderConstants {
        public:
            ShaderConstants(std::vector<std::size_t>, const void*);
            ShaderConstants(const ShaderConstants&) = delete;
            ShaderConstants(ShaderConstants&&) noexcept = delete;
            ~ShaderConstants();

            const std::vector<std::size_t>& getVariablesSize() const;

            std::size_t sumVariablesSize() const;
            void* getData() const;

        private:
            std::vector<std::size_t> variablesSize;
            void* data;
    };

}
