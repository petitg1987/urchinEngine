#ifndef URCHINENGINE_SHADERCONSTANTS_H
#define URCHINENGINE_SHADERCONSTANTS_H

#include <vector>

namespace urchin {

    class ShaderConstants {
        public:
            ShaderConstants(std::vector<std::size_t>, void*);
            ShaderConstants(const ShaderConstants&) = delete;
            ShaderConstants(ShaderConstants&&) noexcept = delete;

            const std::vector<std::size_t>& getVariablesSize() const;

            std::size_t sumVariablesSize() const;
            void* getData() const;

        private:
            std::vector<std::size_t> variablesSize;
            void* data;
    };

}

#endif
